/*******************************************************************************
 * Copyright (c) 2017 - 2020 fortiss GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alexander Tepaev - initial implementation and documentation
 *******************************************************************************/

#include "bacnet_service_handle.h"
#include "bacnet_service_config_fb.h"

CBacnetServiceHandle::CBacnetServiceHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : forte::core::io::IOHandle(controller, direction, type), CExternalEventHandler(paDeviceExecution), m_enHandleState(Idle), mConfigFB(paServiceConfigFB), mValue(new CIEC_ANY())
{
}

CBacnetServiceHandle::~CBacnetServiceHandle()
{
}
