/**
 * This file is part of ldm.
 *
 * Copyright (C) 2011-2016 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * ldm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ldm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ldm. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LDM_OPTICALDRIVE_H_
#define LDM_OPTICALDRIVE_H_

#include <ldm/RemovableDrive>


namespace LDM {

class PLATFORM_MAKE_PUBLIC OpticalDrive : public RemovableDrive
{
public:
	struct Details : public Drive::Details
	{
		bool writeCacheEnabled;
		bool canDetach;
		bool canSpindown;
		bool isBlank;
		bool isAppendable;
		bool isClosed;
		quint32 numTracks;
		quint32 numAudioTracks;
		quint32 numSessions;
	};

public:
	OpticalDrive(const Id &id,
			  	 const QIcon &icon,
			  	 const QString &label,
			  	 bool hidden,
			  	 Device *parent,
			  	 quint64 size,
			  	 MediaTypeSet mediaCompatibility,
			  	 MediaType media,
			  	 bool detachable,
			  	 bool ejectable);

	bool isEjectable() const { return m_ejectable; }
	void setEjectable(bool ejectable) { m_ejectable = ejectable; }

	static const MediaTypeSet &mediaTypeSet();
    virtual void accept(Visitor &visitor);

private:
	bool m_ejectable;
};

}

#endif /* LDM_OPTICALDRIVE_H_ */
