#ifndef LICENSECHECK_H
#define LICENSECHECK_H
#include "../libs/CodeMeter/CodeMeter.h"
#include "../ProjectFiles/AppVersion.h"

const unsigned int BR_CM_APP_BOSS = 3259302;        // Product Code
const unsigned int BR_CM_FIRMCODE = 101966;         // Firm Code

class LicenseCheck
{

public:
    LicenseCheck();
    ~LicenseCheck();

    bool isThereALicense();
    void getLicenseVersion(int *major, int * minor);
    
private:
    
    bool m_bLicenseAvailable;
    int m_nMajorVer;
    int m_nMinorVer;
};



#endif // LICENSECHECK_H