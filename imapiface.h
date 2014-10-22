#ifndef IMAPIFACE_H_INCLUDED
#define IMAPIFACE_H_INCLUDED

#include <vmime/vmime.hpp>
#include <vmime/platforms/posix/posixHandler.hpp>
#include <string>
#include <iostream>

using namespace std;

// Global session object
static vmime::ref <vmime::net::session> g_session = vmime::create <vmime::net::session>();
class NonInteractiveCertificateVerifier : public vmime::security::cert::defaultCertificateVerifier
{
    public:

        void verify(vmime::ref <vmime::security::cert::certificateChain> chain);

    private:

        static std::vector <vmime::ref <vmime::security::cert::X509Certificate> > m_trustedCerts;
};

std::string findAvailableProtocols(const vmime::net::service::Type type);


#endif // IMAPIFACE_H_INCLUDED
