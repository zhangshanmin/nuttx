/****************************************************************************
 * fs/driver/fs_findmtddriver.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/fs/fs.h>

#include "inode/inode.h"

#ifdef CONFIG_MTD

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: find_mtddriver
 *
 * Description:
 *   Return the inode of the named MTD driver specified by 'pathname'
 *
 * Input Parameters:
 *   pathname   - the full path to the named MTD driver to be located
 *   ppinode    - address of the location to return the inode reference
 *
 * Returned Value:
 *   Returns zero on success or a negated errno on failure:
 *
 *   ENOENT  - No MTD driver of this name is registered
 *   ENOTBLK - The inode associated with the pathname is not an MTD driver
 *
 ****************************************************************************/

int find_mtddriver(FAR const char *pathname, FAR struct inode **ppinode)
{
  struct inode_search_s desc;
  FAR struct inode *inode;
  int ret = 0; /* Assume success */

  DEBUGASSERT(pathname != NULL || ppinode != NULL);

  /* Find the inode registered with this pathname */

  SETUP_SEARCH(&desc, pathname, false);

  ret = inode_find(&desc);
  if (ret < 0)
    {
      ferr("ERROR: Failed to find %s\n", pathname);
      ret = -ENOENT;
      goto errout_with_search;
    }

  /* Get the search results */

  inode = desc.node;

  /* Verify that the inode is a block driver. */

  if (!INODE_IS_MTD(inode))
    {
      ferr("ERROR: %s is not a named MTD driver\n", pathname);
      ret = -ENOTBLK;
      goto errout_with_inode;
    }

  /* Return the MTD inode reference */

  DEBUGASSERT(inode->u.i_mtd != NULL);

  *ppinode = inode;
  RELEASE_SEARCH(&desc);
  return OK;

errout_with_inode:
  inode_release(inode);

errout_with_search:
  RELEASE_SEARCH(&desc);
  return ret;
}

#else

int find_mtddriver(FAR const char *pathname, FAR struct inode **ppinode)
{
  return -ENODEV;
}

#endif /* CONFIG_MTD */
