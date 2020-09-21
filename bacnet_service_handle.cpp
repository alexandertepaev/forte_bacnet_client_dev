/*******************************************************************************
 * Copyright (c) 2020 Alexander Tepaev github.com/alexandertepaev
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alexander Tepaev
 *******************************************************************************/
#include "bacnet_service_handle.h"
#include "bacnet_service_config_fb.h"

CBacnetServiceHandle::CBacnetServiceHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : forte::core::io::IOHandle(controller, direction, type), CExternalEventHandler(paDeviceExecution), mConfigFB(paServiceConfigFB),  m_enHandleState(Idle), mValue(new CIEC_ANY())
{
}

CBacnetServiceHandle::~CBacnetServiceHandle()
{
}
