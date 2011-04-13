#include "KDSoapClientInterface.h"
#include "KDSoapMessage.h"
#include "KDSoapValue.h"
#include "KDSoapPendingCallWatcher.h"
#include "KDSoapAuthentication.h"
#include "KDDateTime.h"
#include "wsdl_groupwise.h"
#include "httpserver_p.h"
#include <QtTest/QtTest>
#include <QEventLoop>
#include <QDebug>

using namespace KDSoapUnitTestHelpers;

static const char* xmlEnvBegin =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<soap:Envelope"
        " xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""
        " xmlns:soap-enc=\"http://schemas.xmlsoap.org/soap/encoding/\""
        " xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\""
        " xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\""
        " soap:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"";
static const char* xmlEnvEnd = "</soap:Envelope>";

class GroupwiseTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testGeneratedMethods()
    {
        // No runtime test yet, just checking that the methods got generated
        if (false) { // Don't contact localhost:8080 :-)
            GroupwiseService::GroupWiseBinding groupwise;
            METHODS__AcceptRequest acceptRequest;
            acceptRequest.setComment(QString::fromLatin1("Comment"));
            METHODS__AcceptResponse response = groupwise.acceptRequest(acceptRequest);
            (void)response.status();
        }
    }

    // Note: this test uses "internal" API, serialize/deserialize... not a good example.
    void testStringBaseType()
    {
        TYPES__ContainerRef cref(QString::fromLatin1("str"));
        cref.setDeleted(QDateTime(QDate(2010, 12, 31))); // implicit conversion from QDateTime to KDDateTime
        const KDSoapValue v = cref.serialize(QLatin1String("container"));

        //qDebug() << v.toXml();

        TYPES__ContainerRef cref2;
        cref2.deserialize(v);
        QCOMPARE(cref.value(), cref2.value());
        QCOMPARE(cref.deleted(), cref2.deleted());
    }

    void testBase64()
    {
        HttpServerThread server(updateVersionStatusResponse(), HttpServerThread::Public);
        GroupwiseService::GroupWiseBinding groupwise;
        groupwise.setEndPoint(server.endPoint());

        METHODS__UpdateVersionStatusRequest req;
        req.setEvent(TYPES__VersionEventType::Archive);
        req.setId(QString::fromLatin1("TheId"));
        TYPES__SignatureData sigData;
        sigData.setData("ABCDEF");
        sigData.setSize(6);
        req.setPart(sigData);
        METHODS__UpdateVersionStatusResponse response = groupwise.updateVersionStatusRequest(req);

        // Check what we sent
        QByteArray expectedRequestXml =
            QByteArray(xmlEnvBegin) +
            "><soap:Body>"
            "<n1:updateVersionStatusRequest xmlns:n1=\"http://schemas.novell.com/2005/01/GroupWise/groupwise.wsdl\">"
                "<n1:id>TheId</n1:id>"
                "<n1:event>archive</n1:event>"
                "<n1:part><n1:size>6</n1:size><n1:data>QUJDREVG</n1:data></n1:part>"
            "</n1:updateVersionStatusRequest>"
            "</soap:Body>" + xmlEnvEnd
            + '\n'; // added by QXmlStreamWriter::writeEndDocument
        QVERIFY(xmlBufferCompare(server.receivedData(), expectedRequestXml));

        // The response didn't have the expected fields. Not sure how we should handle that.
        // Right now there's no error, just an "empty" response data.

        QCOMPARE(response.status().code(), 0);
    }

    void testDateTimeInRequest()
    {
        HttpServerThread server(updateVersionStatusResponse(), HttpServerThread::Public);
        GroupwiseService::GroupWiseBinding groupwise;
        groupwise.setEndPoint(server.endPoint());

        METHODS__SetTimestampRequest req;
        KDDateTime frenchDate(QDateTime(QDate(2011, 03, 15), QTime(4, 3, 2, 1)));
        frenchDate.setTimeZone(QString::fromLatin1("+01:00"));
        req.setBackup(frenchDate);

        // implicit conversion from KDDateTime to QDateTime
        req.setRetention(QDateTime(QDate(2011, 01, 15), QTime(4, 3, 2, 1)));

        /*METHODS__SetTimestampResponse response = */groupwise.setTimestampRequest(req);

        // Check what we sent
        QByteArray expectedRequestXml =
            QByteArray(xmlEnvBegin) +
            "><soap:Body>"
            "<n1:setTimestampRequest xmlns:n1=\"http://schemas.novell.com/2005/01/GroupWise/groupwise.wsdl\">"
                "<n1:backup>2011-03-15T04:03:02.001+01:00</n1:backup>"
                "<n1:retention>2011-01-15T04:03:02.001</n1:retention>"
            "</n1:setTimestampRequest>"
            "</soap:Body>" + xmlEnvEnd
            + '\n'; // added by QXmlStreamWriter::writeEndDocument
        QVERIFY(xmlBufferCompare(server.receivedData(), expectedRequestXml));
    }

    void testDateTimeInResponse()
    {
        HttpServerThread server(getTimestampResponse(), HttpServerThread::Public);
        GroupwiseService::GroupWiseBinding groupwise;
        groupwise.setEndPoint(server.endPoint());

        METHODS__GetTimestampRequest req;
        req.setBackup(true);
        req.setRetention(true);
        METHODS__GetTimestampResponse response = groupwise.getTimestampRequest(req);

        // Check what we sent
        QByteArray expectedRequestXml =
            QByteArray(xmlEnvBegin) +
            "><soap:Body>"
            "<n1:getTimestampRequest xmlns:n1=\"http://schemas.novell.com/2005/01/GroupWise/groupwise.wsdl\">"
                "<n1:backup>true</n1:backup>"
                "<n1:retention>true</n1:retention>"
                "<n1:noop>false</n1:noop>"
            "</n1:getTimestampRequest>"
            "</soap:Body>" + xmlEnvEnd
            + '\n'; // added by QXmlStreamWriter::writeEndDocument
        QVERIFY(xmlBufferCompare(server.receivedData(), expectedRequestXml));

        // Check response parsing
        QCOMPARE(response.backup().toString(Qt::ISODate), QString::fromLatin1("2011-03-15T04:03:02")); // Qt doesn't show msec
        QCOMPARE(response.backup().timeZone(), QString::fromLatin1("+01:00"));
        QCOMPARE(response.backup().toDateString(), QString::fromLatin1("2011-03-15T04:03:02.001+01:00"));
        QCOMPARE(response.retention().toDateString(), QString::fromLatin1("2011-01-15T04:03:02.001Z"));
        QCOMPARE(response.retention().timeZone(), QString::fromLatin1("Z"));
    }

private:

    // Bogus response
    static QByteArray updateVersionStatusResponse() {
        return QByteArray(xmlEnvBegin) + " xmlns:gw=\"http://schemas.novell.com/2005/01/GroupWise/groupwise.wsdl\"><soap:Body>"
              "<queryResponse>"
               "<result>"
                "<done>true</done>"
                "<size>3</size>"
               "</result>"
              "</queryResponse>"
              "</soap:Body>" + xmlEnvEnd;
    }

    static QByteArray getTimestampResponse() {
        return QByteArray(xmlEnvBegin) + " xmlns:gw=\"http://schemas.novell.com/2005/01/GroupWise/groupwise.wsdl\"><soap:Body>"
              "<gw:getTimestampResponse>"
                "<gw:backup>2011-03-15T04:03:02.001+01:00</gw:backup>"
                "<gw:retention>2011-01-15T04:03:02.001Z</gw:retention>"
               "</gw:getTimestampResponse>"
              "</soap:Body>" + xmlEnvEnd;
    }

};

QTEST_MAIN(GroupwiseTest)

#include "test_groupwise_wsdl.moc"