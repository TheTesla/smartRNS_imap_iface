
#include "imapiface.h"


// Certificate verifier (TLS/SSL)


void NonInteractiveCertificateVerifier::verify(vmime::ref <vmime::security::cert::certificateChain> chain)
{
    try
    {
        setX509TrustedCerts(m_trustedCerts);
        defaultCertificateVerifier::verify(chain);
    }
    catch (vmime::exceptions::certificate_verification_exception&)
    {
        // Obtain subject's certificate
        vmime::ref <vmime::security::cert::certificate> cert = chain->getAt(0);

        std::cout << std::endl;
        std::cout << "Server sent a '" << cert->getType() << "'" << " certificate." << std::endl;

        // Accept it, and remember user's choice for later
        if (cert->getType() == "X.509") {
            m_trustedCerts.push_back(cert.dynamicCast <vmime::security::cert::X509Certificate>());
        }
    }
}


std::vector <vmime::ref <vmime::security::cert::X509Certificate> > NonInteractiveCertificateVerifier::m_trustedCerts;



std::string findAvailableProtocols(const vmime::net::service::Type type)
{
	vmime::net::serviceFactory* sf = vmime::net::serviceFactory::getInstance();

	std::ostringstream res;
	int count = 0;

	for (int i = 0 ; i < sf->getServiceCount() ; ++i)
	{
		const vmime::net::serviceFactory::registeredService& serv = *sf->getServiceAt(i);

		if (serv.getType() == type)
		{
			if (count != 0)
				res << ", ";

			res << serv.getName();
			++count;
		}
	}

	return res.str();
}
