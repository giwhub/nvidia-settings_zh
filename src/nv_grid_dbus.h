/*
 * Copyright (C) 2017 NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses>.
 *
 * nv_grid_dbus.h
 */

#ifndef _NVIDIA_NV_GRID_DBUS_H_
#define _NVIDIA_NV_GRID_DBUS_H_

/*
 * Details to communicate with nvidia-gridd using dbus mechanism
 */
#define NV_GRID_DBUS_CLIENT             "nvidia.grid.client"
#define NV_GRID_DBUS_TARGET             "nvidia.grid.server"
#define NV_GRID_DBUS_OBJECT             "/nvidia/grid/license"
#define NV_GRID_DBUS_INTERFACE          "nvidia.grid.license"
#define NV_GRID_DBUS_METHOD             "GridLicenseState"
#define LICENSE_DETAILS_UPDATE_SUCCESS  0
#define LICENSE_STATE_REQUEST           1
#define LICENSE_DETAILS_UPDATE_REQUEST  2
#define LICENSE_FEATURE_TYPE_REQUEST    3

/*
 * GRID license states
 */
typedef enum
{
    NV_GRID_UNLICENSED = 0,
    NV_GRID_LICENSE_REQUESTING,
    NV_GRID_LICENSE_FAILED,
    NV_GRID_LICENSED,
    NV_GRID_LICENSE_RENEWING,
    NV_GRID_LICENSE_RENEW_FAILED,
    NV_GRID_LICENSE_EXPIRED,
} gridLicenseState;

/*
 * GRID license feature types
 */
typedef enum
{
    NV_GRID_LICENSE_FEATURE_TYPE_VAPP = 0,
    NV_GRID_LICENSE_FEATURE_TYPE_VGPU,
    NV_GRID_LICENSE_FEATURE_TYPE_QDWS,
} gridLicenseFeatureType;

#endif // _NVIDIA_NV_GRID_DBUS_H_
