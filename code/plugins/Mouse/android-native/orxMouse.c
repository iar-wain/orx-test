/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2011 Orx-Project
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */

/**
 * @file orxMouse.c
 * @date 13/01/2011
 * @author simons.philippe@gmail.com
 *
 * Android mouse plugin implementation
 *
 */


#include "orxPluginAPI.h"

/** Module flags
 */
#define orxMOUSE_KU32_STATIC_FLAG_NONE        0x00000000 /**< No flags */

#define orxMOUSE_KU32_STATIC_FLAG_READY       0x00000001 /**< Ready flag */

#define orxMOUSE_KU32_STATIC_MASK_ALL         0xFFFFFFFF /**< All mask */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Static structure
 */
typedef struct __orxMOUSE_STATIC_t
{
  orxU32      u32Flags;
  orxBOOL     bIsClicked;
  orxVECTOR   vMouseMove, vMousePosition;

} orxMOUSE_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
static orxMOUSE_STATIC sstMouse;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

extern struct engine engine;

/** Event handler
 */
static orxSTATUS orxFASTCALL orxMouse_Android_EventHandler(const orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Depending on ID */
  switch(_pstEvent->eID)
  {
    /* Touch? */
    case orxSYSTEM_EVENT_TOUCH_BEGIN:
    case orxSYSTEM_EVENT_TOUCH_MOVE:
    case orxSYSTEM_EVENT_TOUCH_END:
    {
      orxVECTOR vNewPosition;
  		orxSYSTEM_EVENT_PAYLOAD *pstPayload;
      orxBOOL bActive = orxFALSE;

  		/* Gets payload */
  		pstPayload = (orxSYSTEM_EVENT_PAYLOAD *) _pstEvent->pstPayload;

  		/* Gets new position */
  		orxVector_Set(&vNewPosition, orx2F(pstPayload->stTouch.fX), orx2F(pstPayload->stTouch.fY), orxFLOAT_0);

      /* Updates mouse move */
      orxVector_Sub(&(sstMouse.vMouseMove), &(sstMouse.vMouseMove), &(sstMouse.vMousePosition));
      orxVector_Add(&(sstMouse.vMouseMove), &(sstMouse.vMouseMove), &vNewPosition);

      /* Updates mouse position */
      orxVector_Copy(&(sstMouse.vMousePosition), &vNewPosition);

      if(_pstEvent->eID == orxSYSTEM_EVENT_TOUCH_BEGIN || _pstEvent->eID == orxSYSTEM_EVENT_TOUCH_MOVE) {
        bActive = orxTRUE;
      }
    
      /* Updates click status */
      sstMouse.bIsClicked = bActive;
      break;
    }
  }

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxMouse_Android_ShowCursor(orxBOOL _bShow)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Shows cursor */
  orxDEBUG_PRINT(orxDEBUG_LEVEL_MOUSE, "Not available on this platform!");

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxMouse_Android_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Was already initialized. */
  if(!(sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstMouse, sizeof(orxMOUSE_STATIC));

    /* Adds our mouse event handlers */
    if((eResult = orxEvent_AddHandler(orxEVENT_TYPE_SYSTEM, orxMouse_Android_EventHandler)) != orxSTATUS_FAILURE)
    {
      /* Updates status */
      sstMouse.u32Flags |= orxMOUSE_KU32_STATIC_FLAG_READY;

      /* Sets config section */
      orxConfig_PushSection(orxMOUSE_KZ_CONFIG_SECTION);

      /* Has show cursor value? */
      if(orxConfig_HasValue(orxMOUSE_KZ_CONFIG_SHOW_CURSOR) != orxFALSE)
      {
        /* Updates cursor status */
        orxMouse_Android_ShowCursor(orxConfig_GetBool(orxMOUSE_KZ_CONFIG_SHOW_CURSOR));
      }
  
      /* Pops config section */
      orxConfig_PopSection();
    }
  }

  /* Done! */
  return eResult;
}

void orxFASTCALL orxMouse_Android_Exit()
{
  /* Was initialized? */
  if(sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY)
  {
    /* Removes event handler */
    orxEvent_RemoveHandler(orxEVENT_TYPE_SYSTEM, orxMouse_Android_EventHandler);

    /* Cleans static controller */
    orxMemory_Zero(&sstMouse, sizeof(orxMOUSE_STATIC));
  }

  return;
}

orxSTATUS orxFASTCALL orxMouse_Android_SetPosition(const orxVECTOR *_pvPosition)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);

  /* Updates mouse move */
  orxVector_Sub(&(sstMouse.vMouseMove), &(sstMouse.vMouseMove), &(sstMouse.vMousePosition));
  orxVector_Add(&(sstMouse.vMouseMove), &(sstMouse.vMouseMove), _pvPosition);

  /* Updates position */
  orxVector_Set(&(sstMouse.vMousePosition), _pvPosition->fX, _pvPosition->fY, orxFLOAT_0);

  /* Done! */
  return eResult;
}

orxVECTOR *orxFASTCALL orxMouse_Android_GetPosition(orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult = _pvPosition;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvPosition != orxNULL);

  /* Updates result */
  orxVector_Copy(_pvPosition, &(sstMouse.vMousePosition));

  /* Done! */
  return pvResult;
}

orxBOOL orxFASTCALL orxMouse_Android_IsButtonPressed(orxMOUSE_BUTTON _eButton)
{
  orxBOOL bResult = orxFALSE;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);
  orxASSERT(_eButton < orxMOUSE_BUTTON_NUMBER);

  /* Left button? */
  if(_eButton == orxMOUSE_BUTTON_LEFT)
  {
    /* Updates result */
    bResult = sstMouse.bIsClicked;
  }
  else
  {
    /* Not available */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_MOUSE, "<%s> is not available on this platform!", orxMouse_GetButtonName(_eButton));
  }

  /* Done! */
  return bResult;
}

orxVECTOR *orxFASTCALL orxMouse_Android_GetMoveDelta(orxVECTOR *_pvMoveDelta)
{
  orxVECTOR *pvResult = _pvMoveDelta;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvMoveDelta != orxNULL);

  /* Updates result */
  orxVector_Copy(_pvMoveDelta, &(sstMouse.vMouseMove));

  /* Clears move */
  orxVector_Copy(&(sstMouse.vMouseMove), &orxVECTOR_0);

  /* Done! */
  return pvResult;
}

orxFLOAT orxFASTCALL orxMouse_Android_GetWheelDelta()
{
  orxFLOAT fResult = orxFLOAT_0;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);

  /* Not available */
  orxDEBUG_PRINT(orxDEBUG_LEVEL_MOUSE, "Not available on this platform!");

  /* Done! */
  return fResult;
}


/***************************************************************************
 * Plugin related                                                          *
 ***************************************************************************/

orxPLUGIN_USER_CORE_FUNCTION_START(MOUSE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_Android_Init, MOUSE, INIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_Android_Exit, MOUSE, EXIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_Android_SetPosition, MOUSE, SET_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_Android_GetPosition, MOUSE, GET_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_Android_IsButtonPressed, MOUSE, IS_BUTTON_PRESSED);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_Android_GetMoveDelta, MOUSE, GET_MOVE_DELTA);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_Android_GetWheelDelta, MOUSE, GET_WHEEL_DELTA);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_Android_ShowCursor, MOUSE, SHOW_CURSOR);
orxPLUGIN_USER_CORE_FUNCTION_END();