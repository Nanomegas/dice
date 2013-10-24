///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "FirmwareProgress.h"

FWProgressBackgroundThread::FWProgressBackgroundThread(tcat::dice::FL_OP_ENUM operation_, String file_, String sector_, tcat::uint32 size_, event_device_ref device_, bool requires_entry_point_)
    : ThreadWithProgressWindow ("Firmware progress...",
                                true,
                                false),
	m_operation(operation_),
	m_file(file_),
	m_sector(sector_),
	m_size(size_),
	m_device_ref(device_),
	m_requires_entrypoint (requires_entry_point_)
{
	String msg = String::empty;
	switch (m_operation)
	{
		case tcat::dice::FL_OP_DELETE_IMAGE:
			msg.append("Deleting sector " + m_sector + " ...", 40);
			break;
		case tcat::dice::FL_OP_CREATE_IMAGE:
			msg.append("Creating sector " + m_sector + " ...", 40);
			break;
		case tcat::dice::FL_OP_UPLOAD:
			msg.append("Uploading file " + m_file + " ...", 300);
			break;
		case tcat::dice::FL_OP_READ_MEMORY:
			msg.append("Downloading entire flash to file " + m_file + " ...", 300);
			break;
		
		default:
			msg.append("...", 4);
			break;
	}
	setStatusMessage (msg);
}

FWProgressBackgroundThread::~FWProgressBackgroundThread()
{
	if (m_device_ref) m_device_ref->detach(this);
}

void FWProgressBackgroundThread::run()
{
	if (m_device_ref)
	{
		try
		{
			switch (m_operation)
			{
				case tcat::dice::FL_OP_DELETE_IMAGE:
					m_device_ref->firmware_delete(std::string(m_sector.toUTF8()));
					break;
				case tcat::dice::FL_OP_CREATE_IMAGE:
					if (m_requires_entrypoint)
						m_device_ref->firmware_create(std::string(m_sector.toUTF8()), m_size);
					else
						m_device_ref->firmware_create(std::string(m_sector.toUTF8()), m_size, 0, 0);
					break;
				case tcat::dice::FL_OP_UPLOAD:
					m_device_ref->firmware_upload(std::string(m_file.toUTF8()), std::string(m_sector.toUTF8())); // use 8-bit checksum
					sendActionMessage("fw upload success");
					break;
				case tcat::dice::FL_OP_READ_MEMORY:
					m_device_ref->firmware_download(std::string(m_file.toUTF8()));
					break;
				
				default:
					break;
			}
		}
		catch (tcat::dice::xptn_device_firmware)
		{
			AlertWindow::showMessageBox (AlertWindow::NoIcon,
				"File Upload",
				"\n\nFirmware upload checksum failed.\nThe device firmware will not be changed.\n\nPlease try again and contact the device\nmanufacturer if the problem persists.");
		}
		catch (tcat::dice::xptn_internal exception)
		{
			AlertWindow::showMessageBox (AlertWindow::NoIcon,
				"File Upload",
				"\n\nDevice communications error.\nThe device firmware will not be changed.\n\nPlease try again and contact the device\nmanufacturer if the problem persists.");
		}
		catch (...)
		{
			AlertWindow::showMessageBox (AlertWindow::NoIcon,
				"File Upload",
				"\n\nError.\n\nThe device firmware will not be changed.\n\nPlease try again and contact the device\nmanufacturer if the problem persists.");
		}
	}
}    
void FWProgressBackgroundThread::firmware_progress(tcat::uint32 percent, tcat::dice::FL_OP_ENUM operation) const
{
	const_cast <FWProgressBackgroundThread *>(this)->setProgress ((float)percent/100.0);
}

