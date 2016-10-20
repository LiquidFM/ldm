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

#include "../ldm_Devices.h"
#include "ldm_Devices_p.h"


static Devices *s_instance;


Devices::Devices() :
	d(new Private(this))
{
	Q_ASSERT(s_instance == NULL);
	s_instance = this;
	d->refresh();
}

Devices::~Devices()
{
    Q_ASSERT(s_instance != NULL);
    s_instance = NULL;
	delete d;
}

Devices::const_iterator Devices::begin() const
{
	return d->m_topLevelDevices.constBegin();
}

Devices::const_iterator Devices::end() const
{
	return d->m_topLevelDevices.constEnd();
}

void Devices::accept(LDM::Visitor &visitor)
{
    for (auto i : *this)
        i->accept(visitor);
}

Devices &Devices::instance()
{
    Q_ASSERT(s_instance != NULL);
	return *s_instance;
}

void Devices::slotDeviceAdded(const LDM::Device *device)
{

}

void Devices::slotDeviceRemoved(const LDM::Device *device)
{

}

void Devices::slotDeviceChanged(const LDM::Device *device)
{

}
