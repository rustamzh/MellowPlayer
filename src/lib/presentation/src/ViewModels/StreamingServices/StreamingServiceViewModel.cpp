#include <MellowPlayer/Presentation/ViewModels/StreamingServices/StreamingServiceViewModel.hpp>
#include <MellowPlayer/Domain/Player/Player.hpp>
#include <MellowPlayer/Domain/Player/Players.hpp>
#include <MellowPlayer/Domain/Settings/ISettingsStore.hpp>
#include <MellowPlayer/Domain/StreamingServices/StreamingService.hpp>
#include <MellowPlayer/Domain/StreamingServices/StreamingServiceScript.hpp>
#include <MellowPlayer/Domain/Settings/SettingKey.hpp>
#include <MellowPlayer/Infrastructure/Network/NetworkProxy.hpp>
#include <MellowPlayer/Infrastructure/Network/NetworkProxies.hpp>
#include <qfile.h>

using namespace std;
using namespace MellowPlayer::Domain;
using namespace MellowPlayer::Domain;
using namespace MellowPlayer::Infrastructure;
using namespace MellowPlayer::Presentation;

#define DEFAULT_ZOOM_FACTOR 7

StreamingServiceViewModel::StreamingServiceViewModel(StreamingService& streamingService, 
                                                     ISettingsStore& settingsStore,
                                                     IUserScriptFactory& factory,
                                                     Players& players,
                                                     INetworkProxies& networkProxies,
                                                     QObject* parent) : 
        QObject(parent),
        networkProxy_(networkProxies.get(streamingService.name())),
        streamingService_(streamingService), 
        settingsStore_(settingsStore),
        player_(players.get(streamingService.name())),
        userScriptsViewModel_(streamingService.name(), factory, settingsStore, this),
        zoomFactor_(settingsStore_.value(zoomFactorSettingsKey(), 7).toInt()),
        isActive_(false),
        previewImageUrl_("qrc:/MellowPlayer/Presentation/images/home-background.png")
{
    connect(&streamingService_, &StreamingService::scriptChanged, this, &StreamingServiceViewModel::sourceCodeChanged);
    Q_ASSERT(networkProxy_ != nullptr);
}

QString StreamingServiceViewModel::logo() const
{
    return streamingService_.logo();
}

QString StreamingServiceViewModel::name() const
{
    return streamingService_.name();
}

Player* StreamingServiceViewModel::player()
{
    return player_.get();
}

QString StreamingServiceViewModel::url() const
{
    QString customUrl = settingsStore_.value(customUrlSettingsKey(), "").toString();
    return customUrl.isEmpty() ? streamingService_.url() : customUrl;
}

QString StreamingServiceViewModel::version() const
{
    return streamingService_.version();
}

QString StreamingServiceViewModel::authorName() const
{
    return streamingService_.author();
}

QString StreamingServiceViewModel::authorWebsite() const
{
    return streamingService_.authorWebsite();
}

bool StreamingServiceViewModel::operator==(const StreamingServiceViewModel& rhs) const
{
    return streamingService_ == rhs.streamingService_;
}

bool StreamingServiceViewModel::operator!=(const StreamingServiceViewModel& rhs) const
{
    return !operator==(rhs);
}

StreamingService* StreamingServiceViewModel::streamingService() const
{
    return &streamingService_;
}

int StreamingServiceViewModel::sortOrder() const
{
    return settingsStore_.value(sortOrderSettingsKey(), "-1").toInt();
}

void StreamingServiceViewModel::setSortOrder(int newOrder)
{
    settingsStore_.setValue(sortOrderSettingsKey(), newOrder);
    emit sortOrderChanged();
}

bool StreamingServiceViewModel::isEnabled() const
{
    return settingsStore_.value(isEnabledSettingsKey(), "true").toBool();
}

void StreamingServiceViewModel::setEnabled(bool enabled)
{
    if (enabled != isEnabled()) {
        settingsStore_.setValue(isEnabledSettingsKey(), enabled);
        emit isEnabledChanged();

        if (!enabled) {
            player_->suspend();
            setSortOrder(255);
        }
    }
}

void StreamingServiceViewModel::setUrl(const QString& newUrl)
{
    if (newUrl != url()) {
        settingsStore_.setValue(customUrlSettingsKey(), newUrl);
        emit urlChanged(newUrl);
    }
}

void StreamingServiceViewModel::setActive(bool isActive)
{
    if (isActive_ == isActive)
        return;

    isActive_ = isActive;
    if (!isActive_)
        setPreviewImageUrl("qrc:/MellowPlayer/Presentation/images/home-background.png");
    emit isActiveChanged();
}

void StreamingServiceViewModel::setPreviewImageUrl(QString previewImageUrl)
{
    if (previewImageUrl_ == previewImageUrl)
        return;

    previewImageUrl_ = previewImageUrl;
    emit previewImageUrlChanged();
}

QString StreamingServiceViewModel::customUrlSettingsKey() const
{
    return streamingService_.name() + "/url";
}

QString StreamingServiceViewModel::sortOrderSettingsKey() const
{
    return streamingService_.name() + "/sortOrder";
}

QString StreamingServiceViewModel::isEnabledSettingsKey() const
{
    return streamingService_.name() + "/isEnabled";
}

QObject* StreamingServiceViewModel::userScripts()
{
    return &userScriptsViewModel_;
}

int StreamingServiceViewModel::zoomFactor() const {
    return zoomFactor_;
}

void StreamingServiceViewModel::setZoomFactor(int zoomFactor) {
    if (zoomFactor_ != zoomFactor) {
        zoomFactor_ = zoomFactor;
        settingsStore_.setValue(zoomFactorSettingsKey(), zoomFactor);
        emit zoomFactorChanged();
    }

}

QString StreamingServiceViewModel::zoomFactorSettingsKey() const {
    return streamingService_.name() + "/zoomFactor";
}

bool StreamingServiceViewModel::notificationsEnabled() const
{
    return settingsStore_.value(notificationsEnabledSettingsKey(), true).toBool();
}

void StreamingServiceViewModel::setNotificationsEnabled(bool value)
{
    if (value != notificationsEnabled())
    {
        settingsStore_.setValue(notificationsEnabledSettingsKey(), value);
        emit notificationsEnabledChanged();
    }
}

bool StreamingServiceViewModel::isActive() const
{
    return isActive_;
}

QString StreamingServiceViewModel::previewImageUrl() const
{
    return previewImageUrl_;
}

QString StreamingServiceViewModel::notificationsEnabledSettingsKey() const
{
    return streamingService_.name() + "/notificationsEnabled";
}

QString StreamingServiceViewModel::sourceCode() const
{
    return streamingService_.script()->constants() + "\n" + streamingService_.script()->code();
}
