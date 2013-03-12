#include "FatxDriveDetection.h"

std::vector<FatxDrive*> FatxDriveDetection::GetAllFatxDrives()
{
    std::vector<FatxDrive*> drives;
    std::vector<HANDLE> devices = getPhysicalDisks();
    DWORD magic;
    DeviceIO *io;

    for (int i = 0; i < devices.size(); i++)
    {
        try
        {
            qDebug() << QString::number((DWORD)devices.at(i));

            io = new DeviceIO(devices.at(i));
            if (io->DriveLength() > HddOffsets::Data)
            {
                io->SetPosition(HddOffsets::Data);
                magic = io->ReadDword();
                qDebug() << "drive leng GOOD - " << QString::number(magic, 16);
                if (magic == FATX_MAGIC)
                {
                    qDebug() << QString::number(devices.size());
                    FatxDrive *drive = new FatxDrive(devices.at(i));
                    drives.push_back(drive);
                }
                else
                    io->Close();
            }

            delete io;
        }
        catch (...)
        {
        }
    }

    return drives;
}

std::vector<HANDLE> FatxDriveDetection::getPhysicalDisks()
{
    std::vector<HANDLE> physicalDiskPaths;
    std::wstringstream ss;

    for (int i = 0; i < 16; i++)
    {
        ss << L"\\\\.\\PHYSICALDRIVE" << i;

        HANDLE drive = CreateFile(ss.str().c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (drive != INVALID_HANDLE_VALUE)
            physicalDiskPaths.push_back(drive);

        ss.str(std::wstring());
    }

    return physicalDiskPaths;
}
