function getArtist() {
    var artists = externalAPI.getCurrentTrack().artists;
    var artistTitle = artists[0].title;
    for (var i = 1; i < artists.length; i++) {
        artistTitle = artistTitle + ", " + artists[i].title;
    }
    return artistTitle;
}

function update() {
    var playbackStatus;
    if (externalAPI.isPlaying())
        playbackStatus = mellowplayer.PlaybackStatus.PLAYING;
    else
        playbackStatus = mellowplayer.PlaybackStatus.PAUSED;


    var track = externalAPI.getCurrentTrack();
    var progress = externalAPI.getProgress();
    var controls = externalAPI.getControls();
    return {
        "playbackStatus": playbackStatus,
        "canSeek": true,
        "canGoNext": controls.next,
        "canGoPrevious": controls.prev,
        "canAddToFavorites": controls.like,
        "volume": externalAPI.getVolume(),
        "duration": Math.floor(progress.duration),
        "position": Math.floor(progress.position),
        "songId": getHashCode(track.title + getArtist()),
        "songTitle": track.title,
        "artistName": getArtist(),
        "albumTitle": track.album.title,
        "artUrl": track.cover.slice(0, -2) + "200x200",
        "isFavorite": track.liked
    };
}

function play() {
    externalAPI.togglePause();
}

function pause() {
    externalAPI.togglePause();
}

function goNext() {
    externalAPI.next();
}

function goPrevious() {
    externalAPI.prev();
}

function setVolume(volume) {
    externalAPI.setVolume(volume);
}

function addToFavorites() {
    externalAPI.toggleLike();
}

function removeFromFavorites() {
    externalAPI.toggleLike();
}

function seekToPosition(position) {
    externalAPI.setPosition(position);
}
