import qbs

Project {
    name: "Windows"
    condition: qbs.targetOS.contains("windows")

    InnoSetup {
        name: "MellowPlayer_Setup"
        files: ["setup.iss"]
        builtByDefault: false

        innosetup.defines: ["MyAppVersion=" + project.version]
        innosetup.verboseOutput: false

        Group {
            name: "InnoSetup Installer"
            fileTagsFilter: ["innosetup.exe"]
            qbs.installDir: "dist"
            qbs.install: true
        }
    }
}
