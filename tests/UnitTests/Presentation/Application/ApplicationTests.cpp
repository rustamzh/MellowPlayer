#include <catch.hpp>
#include <MellowPlayer/Domain/BuildConfig.hpp>
#include <MellowPlayer/Presentation/Application/Application.hpp>
#include <QtTest/QSignalSpy>
#include <QtGui/QSessionManager>
#include "Mocks/QtApplicationMock.hpp"

using namespace testing;
using namespace MellowPlayer::Domain;
using namespace MellowPlayer::Infrastructure;
using namespace MellowPlayer::Presentation;
using namespace MellowPlayer::Presentation::Tests;

SCENARIO("Application tests")
{
    GIVEN("An IQtApplication mock")
    {
        WHEN("Application is created")
        {
            NiceMock<QtApplicationMock> qtApplication;

            THEN("applicationName is set")
            {
                EXPECT_CALL(qtApplication, setApplicationName(QString("MellowPlayer"))).Times(1);
            }

            AND_THEN("applicationDisplayName is set")
            {
                EXPECT_CALL(qtApplication, setApplicationDisplayName(QString("MellowPlayer"))).Times(1);
            }

            AND_THEN("applicationVersion is set")
            {
                EXPECT_CALL(qtApplication, setApplicationVersion(BuildConfig::getVersion())).Times(1);
            }

            AND_THEN("organizationDomain is set")
            {
                EXPECT_CALL(qtApplication, setOrganizationDomain(QString("org.mellowplayer"))).Times(1);
            }

            AND_THEN("organizationName is set")
            {
                EXPECT_CALL(qtApplication, setOrganizationName(QString("MellowPlayer"))).Times(1);
            }

            AND_THEN("windowIcon is set")
            {
                EXPECT_CALL(qtApplication, setWindowIcon(_)).Times(1);
            }

            Application application(qtApplication);
        }

        NiceMock<QtApplicationMock> qtApplication;
        Application application(qtApplication);

        WHEN("qtApplication.commitDataRequest is emitted, application.commitDataRequest is emitted too")
        {
            QSignalSpy spy(&application, &IApplication::commitDataRequest);
            emit qtApplication.commitDataRequest();
            REQUIRE(spy.count() == 1);
        }

        WHEN("initializing the application")
        {
            THEN("font is set")
            {
                EXPECT_CALL(qtApplication, setFont(_)).Times(1);
            }

            AND_THEN("translator is set")
            {
                EXPECT_CALL(qtApplication, installTranslator(_)).Times(1);
            }

            QSignalSpy spy(&application, &IApplication::initialized);
            application.initialize();

            AND_THEN("initialized signal is emitted")
            {
                REQUIRE(spy.count() == 1);
            }
        }

        WHEN("running the application")
        {
            THEN("Qt Application exec method is called")
            {
                EXPECT_CALL(qtApplication, exec()).Times(1);
            }

            QSignalSpy spy(&application, &IApplication::started);
            application.run();

            THEN("started signal is emitted")
            {
                REQUIRE(spy.count() == 1);
            }
        }
    }
}
