#include "../ProjectFiles/licenseCheck.h"
#include <cstring>

LicenseCheck::LicenseCheck() :
    m_bLicenseAvailable(false),
    m_nMajorVer(0),
    m_nMinorVer(0)
{
    CMACCESS cmaccess;
    memset(&cmaccess, 0, sizeof(cmaccess));
    cmaccess.mflCtrl = CM_ACCESS_NOUSERLIMIT;
    cmaccess.mulFirmCode = BR_CM_FIRMCODE;
    cmaccess.mulProductCode = BR_CM_APP_BOSS;

    HCMSysEntry hcm = CmAccess(CM_ACCESS_LOCAL, &cmaccess);

    if (hcm == NULL)    // No dongle found
        return;
    else
    {
        int num = 10;
        CMENTRYDATA cmEntryData[10];
        memset(&cmEntryData, 0, sizeof(cmEntryData));
        
        int res = CmGetInfo(hcm, CM_GEI_ENTRYDATA, &cmEntryData, sizeof(CMENTRYDATA) * num);
        int requiredBytes = num*sizeof(CMENTRYDATA);

        if (res <= requiredBytes)
        {
            for (int i(0); i < num; i++)
            {
                if (cmEntryData[i].mflCtrl == CM_GF_EXTPROTDATA && (cmEntryData[i].mcbData >= 2))
                {
                    int major, minor;
                    major = cmEntryData[i].mabData[0];
                    minor = cmEntryData[i].mabData[1];

                    if ((major >= APP_VERSION_MAJOR) && ((minor >= APP_VERSION_MINOR) || APP_VERSION_MAJOR == 1)) // Don't check the minor for version 1, users get free upgrades for this version
                    {
                        m_nMajorVer = major;
                        m_nMinorVer = minor;
                        m_bLicenseAvailable = true;
                    }
                    break;
                }
            }
            

        }

    }
}

LicenseCheck::~LicenseCheck()
{

}

bool LicenseCheck::isThereALicense()
{
    return true;
    //return m_bLicenseAvailable;
}

void LicenseCheck::getLicenseVersion(int *major, int * minor)
{
    *major = m_nMajorVer;
    *minor = m_nMinorVer;
}

