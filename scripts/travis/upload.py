#!/usr/bin/env python
"""
Upload binaries on the specified github release.

If the specified tag name is "continuous", it will delete any pre-existing release and tag with the name "continuous" and create a new one.

Usage:
    ./upload.py TAG_NAME GLOB_EXPR

    TAG_NAME and GLOB_EXPR are optional. They defaults to "Continuous" and "dist/*".

Examples:

- continuous build:

    ./upload.py Continuous dist/*

- release build

    ./upload.py 2.95.0 dist/*

.. note:: This script depends on github3.py (pip install github3.py)

"""
from datetime import datetime
import glob
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "github3.zip"))
print(sys.path)

from github3 import login, GitHubError

CONTINUOUS_RELEASE_NAME = "Continuous"
CONTINUOUS_RELEASE_DESCRIPTION = """This release contains the artifacts of a continuous build.

*While this is not an official release, this build is considered stable as all unit tests and integration tests passed. Use it if you want to try the latest features!*

"""
DEFAULT_GLOB_EXPR = "dist/*"


def parse_command_line_args():
    if len(sys.argv) == 3:
        tag = sys.argv[1]
        glob_expression = sys.argv[2]
    else:
        tag = CONTINUOUS_RELEASE_NAME
        glob_expression = DEFAULT_GLOB_EXPR

    print("[args] tag: " + tag)
    print("[args] glob-expression: " + glob_expression)
    return tag, glob_expression


def get_repo_slug():
    travis_repo_slug = os.getenv("TRAVIS_REPO_SLUG")
    if travis_repo_slug:
        print("running on Travis CI")
        repo_slug = travis_repo_slug
    else:
        print("not running on travis")
        repo_slug = os.getenv("REPO_SLUG")
        if repo_slug is None:
            try:
                repo_slug = raw_input("Repo slug (GitHub and Travis CI username/reponame): ")
            except NameError:
                repo_slug = input("Repo slug (GitHub and Travis CI username/reponame): ")

    print("repo slug: %r" % repo_slug)
    return repo_slug


def get_github_token():
    github_token = os.getenv("GITHUB_TOKEN")
    if github_token is None:
        try:
            github_token = raw_input("Token (https://github.com/settings/tokens): ")
        except NameError:
            github_token = input("Token (https://github.com/settings/tokens): ")

    return github_token


def get_git_commit():
    commit_sha1 = os.getenv("TRAVIS_COMMIT")
    if commit_sha1 is None:
        commit_sha1 = subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode().replace("\n", "")

    print("commit: %r" % commit_sha1)
    return commit_sha1


def find_release(repo, release_name):
    for release in repo.releases():
        if release.name == release_name:
            print("release found: %s" % release_name)
            return release
    raise ValueError("release not found: " + release_name)


def delete_tag(repo, tag_name):
    subprocess.check_call(['git', 'push', '--delete', 'origin', tag_name])
    subprocess.check_call(['git', 'tag', '--delete', 'tagname', tag_name])


def update_continuous_release(repo, repo_slug, commit):
    try:
        release = find_release(repo, CONTINUOUS_RELEASE_NAME)
    except ValueError as e:
        print(e)
        release = create_continuous_release(repo, repo_slug, commit)
    else:
        if release.target_commitish != commit:
            print("deleting pre-existing release")
            release.delete()
            delete_tag(repo, CONTINUOUS_RELEASE_NAME)
            release = create_continuous_release(repo, repo_slug, commit)
        else:
            print('release is up to date and does not need to be recreated')

    return release


def create_continuous_release(repo, repo_slug, commit):
    travis_build_id = os.getenv('TRAVIS_BUILD_ID')

    description = CONTINUOUS_RELEASE_DESCRIPTION

    if travis_build_id:
        description = description + "\n" + \
                      "Travis CI build log: https://travis-ci.org/" + repo_slug + "/builds/" + travis_build_id + "/"
    print("creating release: %s" % CONTINUOUS_RELEASE_NAME)
    return repo.create_release(
        CONTINUOUS_RELEASE_NAME, commit, CONTINUOUS_RELEASE_NAME, description, prerelease=True)


def upload_binaries(release, glob_expr):
    print('uploading binaries: ' + glob_expr)
    for file in glob.glob(glob_expr):
        with open(file, 'rb') as f:
            data = f.read()

        name = os.path.split(file)[1]
        content_type = "application/octet-stream"

        print(" - uploading asset")
        print("     path: " + file)
        print("     name: " + name)
        release.upload_asset(content_type, name, data)


def get_tag_release(commit, repo, tag):
    try:
        release = find_release(repo, tag)
    except ValueError:
        release = repo.create_release(tag, commit, tag, "", prerelease=False)

    return release


def main():
    tag, glob_expression = parse_command_line_args()
    repo_slug = get_repo_slug()
    owner_name, repository_name = repo_slug.split('/')
    commit = get_git_commit()
    github_token = get_github_token()
    github = login(token=github_token)
    repo = github.repository(owner_name, repository_name)

    if tag == CONTINUOUS_RELEASE_NAME:
        release = update_continuous_release(repo, repo_slug, commit)
    else:
        release = get_tag_release(commit, repo, tag)

    # upload binaries
    upload_binaries(release, glob_expression)


if __name__ == "__main__":
    main()
