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

#ifndef LDM_DEVICES_H_
#define LDM_DEVICES_H_

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <ldm/Device>


class PLATFORM_MAKE_PUBLIC Devices : public QObject
{
    Q_OBJECT

public:
	typedef QMap<LDM::Device::Id, LDM::Device *> Container;
	typedef Container::const_iterator            const_iterator;

public:
	enum Events
	{
		DeviceAdded,
		DeviceRemoved,
		DeviceChanged
	};

public:
	Devices();
	virtual ~Devices();

	const_iterator begin() const;
	const_iterator end() const;
    void accept(LDM::Visitor &visitor);

	static Devices &instance();

private:
	void slotDeviceAdded(const LDM::Device *device);
	void slotDeviceRemoved(const LDM::Device *device);
	void slotDeviceChanged(const LDM::Device *device);

private:
	class Private;
	Private *d;
};

#endif /* LDM_DEVICES_H_ */
