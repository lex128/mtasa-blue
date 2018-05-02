/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaObjectDefs.cpp
*  PURPOSE:     Lua object definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaObjectDefs::LoadFunctions ( void )
{
    // Object create/destroy funcs
    CLuaCFunctions::AddFunction ( "createObject", CreateObject );

    // Object get funcs
    CLuaCFunctions::AddFunction ( "isObjectStatic", IsObjectStatic );
    CLuaCFunctions::AddFunction ( "getObjectScale", GetObjectScale );
    CLuaCFunctions::AddFunction ( "isObjectBreakable", IsObjectBreakable );
    CLuaCFunctions::AddFunction ( "getObjectProperty", GetObjectProperty );
    CLuaCFunctions::AddFunction ( "getObjectProperties", GetObjectProperties );

    // Object set funcs
    CLuaCFunctions::AddFunction ( "moveObject", MoveObject );
    CLuaCFunctions::AddFunction ( "stopObject", StopObject );
    CLuaCFunctions::AddFunction ( "setObjectScale", SetObjectScale );
    CLuaCFunctions::AddFunction ( "setObjectStatic", SetObjectStatic );
    CLuaCFunctions::AddFunction ( "setObjectBreakable", SetObjectBreakable );
    CLuaCFunctions::AddFunction ( "breakObject", BreakObject );
    CLuaCFunctions::AddFunction ( "respawnObject", RespawnObject );
    CLuaCFunctions::AddFunction ( "toggleObjectRespawn", ToggleObjectRespawn );
    CLuaCFunctions::AddFunction ( "setObjectProperty", SetObjectProperty );
}


void CLuaObjectDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createObject" );
    lua_classfunction ( luaVM, "move", "moveObject" );
    lua_classfunction ( luaVM, "stop", "stopObject" );
    lua_classfunction ( luaVM, "break", "breakObject" );
    lua_classfunction ( luaVM, "respawn", "respawnObject" );
    lua_classfunction ( luaVM, "toggleRespawn", "toggleObjectRespawn" );

    lua_classfunction ( luaVM, "getScale", "getObjectScale" );
    lua_classfunction ( luaVM, "isBreakable", "isObjectBreakable" );
    lua_classfunction ( luaVM, "getProperties", "getObjectProperties" );

    lua_classfunction ( luaVM, "setScale", "setObjectScale" );
    lua_classfunction ( luaVM, "setBreakable", "setObjectBreakable" );

    lua_classvariable ( luaVM, "scale", "setObjectScale", "getObjectScale" );
    lua_classvariable ( luaVM, "breakable", "setObjectBreakable", "isObjectBreakable" );

    // Add deprecated methods for backwards compatibility
    lua_classfunction(luaVM, "toggleObjectRespawn", "toggleObjectRespawn");

    lua_registerclass ( luaVM, "Object", "Element" );
}


int CLuaObjectDefs::CreateObject ( lua_State* luaVM )
{
//  object createObject ( int modelid, float x, float y, float z, [float rx, float ry, float rz, bool lowLOD] )
    ushort usModelID;
    CVector vecPosition;
    CVector vecRotation;
    bool bLowLod;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usModelID );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecRotation, vecRotation );
    argStream.ReadBool ( bLowLod, false );

    if ( !argStream.HasErrors () )
    {
        if ( CClientObjectManager::IsValidModel  ( usModelID ) )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CResource* pResource = pLuaMain->GetResource ();
                if ( pResource )
                {
                    CClientObject* pObject = CStaticFunctionDefinitions::CreateObject ( *pResource, usModelID, vecPosition, vecRotation, bLowLod );
                    if ( pObject )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup();
                        if ( pGroup )
                        {
                            pGroup->Add ( ( CClientEntity* ) pObject );
                        }

                        lua_pushelement ( luaVM, pObject );
                        return 1;
                    }
                }
            }
        }
        else
            argStream.SetCustomError( "Invalid model id" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::IsObjectStatic ( lua_State* luaVM )
{
//  bool isObjectStatic ( object theObject )
    CClientObject* pObject;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pObject );

    if ( !argStream.HasErrors () )
    {
        bool bStatic;
        if ( CStaticFunctionDefinitions::IsElementFrozen ( *pObject, bStatic ) )
        {
            lua_pushboolean ( luaVM, bStatic );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::GetObjectScale ( lua_State* luaVM )
{
//  float, float, float getObjectScale ( object theObject )
    CClientObject* pObject;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pObject );

    if ( !argStream.HasErrors () )
    {
        CVector vecScale;
        if ( CStaticFunctionDefinitions::GetObjectScale ( *pObject, vecScale ) )
        {
            lua_pushnumber ( luaVM, vecScale.fX );
            lua_pushnumber ( luaVM, vecScale.fY );
            lua_pushnumber ( luaVM, vecScale.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::IsObjectBreakable ( lua_State* luaVM )
{
//  bool isObjectBreakable ( int modelId )
    
    CScriptArgReader argStream ( luaVM );

    if ( argStream.NextIsNumber () )
    {
        unsigned short usModel;
        argStream.ReadNumber ( usModel );

        lua_pushboolean ( luaVM, CClientObjectManager::IsBreakableModel ( usModel ) );
        return 1;
    }

//  bool isObjectBreakable ( object theObject )
    CClientObject* pObject;

    argStream.ReadUserData ( pObject );
    if ( !argStream.HasErrors () )
    {
        bool bBreakable;
        if ( CStaticFunctionDefinitions::IsObjectBreakable ( *pObject, bBreakable ) )
        {
            lua_pushboolean ( luaVM, bBreakable );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::GetObjectProperty ( lua_State* luaVM )
{
//  float, float, float getObjectProperty ( object theObject, string property )
    CClientObject* pObject;
    eObjectProperty eProp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pObject );
    argStream.ReadEnumString ( eProp, eObjectProperty::OBJECT_PROPERTY_MAX);

    if ( !argStream.HasErrors () )
    {
        switch (eProp)
        {
            case OBJECT_PROPERTY_MASS:
            {
                float fMass;
                if (CStaticFunctionDefinitions::GetObjectMass(*pObject, fMass))
                {
                    lua_pushnumber(luaVM, fMass);
                    return 1;
                }
                break;
            }
            case OBJECT_PROPERTY_TURNMASS:
            {
                float fTurnMass;
                if (CStaticFunctionDefinitions::GetObjectTurnMass(*pObject, fTurnMass))
                {
                    lua_pushnumber(luaVM, fTurnMass);
                    return 1;
                }
                break;
            }
            case OBJECT_PROPERTY_AIRRESISTANCE:
            {
                float fAirResistance;
                if (CStaticFunctionDefinitions::GetObjectAirResistance(*pObject, fAirResistance))
                {
                    lua_pushnumber(luaVM, fAirResistance);
                    return 1;
                }
                break;
            }
            case OBJECT_PROPERTY_ELASTICITY:
            {
                float fElasticity;
                if (CStaticFunctionDefinitions::GetObjectElasticity(*pObject, fElasticity))
                {
                    lua_pushnumber(luaVM, fElasticity);
                    return 1;
                }
                break;
            }
            case OBJECT_PROPERTY_CENTEROFMASS:
            {
                CVector vecCenterOfMass;
                if (CStaticFunctionDefinitions::GetObjectCenterOfMass(*pObject, vecCenterOfMass))
                {
                    lua_pushnumber(luaVM, vecCenterOfMass.fX);
                    lua_pushnumber(luaVM, vecCenterOfMass.fY);
                    lua_pushnumber(luaVM, vecCenterOfMass.fZ);
                    return 3;
                }
                break;
            }
            case OBJECT_PROPERTY_BUOYANCY:
            {
                float fBuoyancyConstant;
                if (CStaticFunctionDefinitions::GetObjectBuoyancyConstant(*pObject, fBuoyancyConstant))
                {
                    lua_pushnumber(luaVM, fBuoyancyConstant);
                    return 1;
                }
                break;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::GetObjectProperties ( lua_State* luaVM )
{
//  table getObjectProperties ( object theObject )
    CClientObject* pObject = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pObject );

    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );

        lua_pushnumber ( luaVM, pObject->GetMass () );
        lua_setfield ( luaVM, -2, "mass" );

        lua_pushnumber ( luaVM, pObject->GetTurnMass () );
        lua_setfield ( luaVM, -2, "turnMass" );

        lua_pushnumber ( luaVM, pObject->GetAirResistance () );
        lua_setfield ( luaVM, -2, "airResistance" );

        lua_pushnumber ( luaVM, pObject->GetElasticity () );
        lua_setfield ( luaVM, -2, "elasticity" );

        lua_createtable ( luaVM, 3, 0 );
        CVector vecCenter;
        pObject->GetCenterOfMass(vecCenter);
        lua_pushnumber ( luaVM, 1 );
        lua_pushnumber ( luaVM, vecCenter.fX );
        lua_settable ( luaVM, -3 );
        lua_pushnumber ( luaVM, 2 );
        lua_pushnumber ( luaVM, vecCenter.fY );
        lua_settable ( luaVM, -3 );
        lua_pushnumber ( luaVM, 3 );
        lua_pushnumber ( luaVM, vecCenter.fZ );
        lua_settable ( luaVM, -3 );
        lua_setfield ( luaVM, -2, "centerOfMass" );

        lua_pushnumber ( luaVM, pObject->GetBuoyancyConstant () );
        lua_setfield ( luaVM, -2, "buoyancy" );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::MoveObject ( lua_State* luaVM )
{
//  bool moveObject ( object theObject, int time, float targetx, float targety, float targetz,
//      [ float moverx, float movery, float moverz, string strEasingType, float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot ] )
    CClientEntity* pEntity; int iTime; CVector vecTargetPosition; CVector vecTargetRotation;
    CEasingCurve::eType easingType; float fEasingPeriod; float fEasingAmplitude; float fEasingOvershoot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( iTime );
    argStream.ReadVector3D ( vecTargetPosition );
    argStream.ReadVector3D ( vecTargetRotation, CVector ( ) );
    argStream.ReadEnumString ( easingType, CEasingCurve::Linear );
    argStream.ReadNumber ( fEasingPeriod, 0.3f );
    argStream.ReadNumber ( fEasingAmplitude, 1.0f );
    argStream.ReadNumber ( fEasingOvershoot, 1.70158f );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::MoveObject ( *pEntity, iTime, vecTargetPosition, vecTargetRotation, easingType, fEasingPeriod, fEasingAmplitude, fEasingOvershoot ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::StopObject ( lua_State* luaVM )
{
//  bool stopObject ( object theobject )
    CClientEntity* pEntity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::StopObject ( *pEntity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::SetObjectScale ( lua_State* luaVM )
{
//  bool setObjectScale ( object theObject, float scale )
    CClientEntity* pEntity;
    CVector vecScale;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );


    // Caz - This function looks totally wrong
    // the function is designed to support the following syntaxes
    // setObjectScale ( obj, 2 ) -- all other components are set to 2
    // setObjectScale ( obj, 2, 1, 5 ) -- custom scaling on 3 axis

    if ( argStream.NextIsVector3D ( ) )
    {
        argStream.ReadVector3D ( vecScale );
    }
    else
    {
        // Caz - Here is what I am talking about.
        argStream.ReadNumber ( vecScale.fX );
        argStream.ReadNumber ( vecScale.fY, vecScale.fX );
        argStream.ReadNumber ( vecScale.fZ, vecScale.fX );
    }
    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetObjectScale ( *pEntity, vecScale ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::SetObjectStatic ( lua_State* luaVM )
{
//  bool setObjectStatic ( object theObject, bool toggle )
    CClientObject* pObject; bool bStatic;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pObject );
    argStream.ReadBool ( bStatic );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetElementFrozen ( *pObject, bStatic ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::SetObjectBreakable ( lua_State* luaVM )
{
    //  bool setObjectBreakable ( object theObject, bool bBreakable )
    CClientEntity* pEntity; 
    bool bBreakable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bBreakable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetObjectBreakable ( *pEntity, bBreakable ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::BreakObject ( lua_State* luaVM )
{
//  bool breakObject( object theObject )
    CClientEntity* pEntity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::BreakObject ( *pEntity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::RespawnObject ( lua_State* luaVM )
{
//  bool respawnObject ( object theObject )
    CClientEntity* pEntity;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RespawnObject ( *pEntity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::ToggleObjectRespawn ( lua_State* luaVM )
{
//  bool toggleObjectRespawn ( object theObject, bool respawn )
    CClientEntity* pEntity; bool bRespawn;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bRespawn );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ToggleObjectRespawn ( *pEntity, bRespawn ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaObjectDefs::SetObjectProperty ( lua_State* luaVM )
{
//  bool setObjectProperty ( object theObject, string property, ... )
    CClientEntity* pEntity;
    eObjectProperty eProp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadEnumString ( eProp, eObjectProperty::OBJECT_PROPERTY_MAX);

    if ( !argStream.HasErrors () )
    {
        switch (eProp)
        {
        case OBJECT_PROPERTY_MASS:
        {
            float fMass;
            argStream.ReadNumber(fMass);
            if (CStaticFunctionDefinitions::SetObjectMass(*pEntity, fMass))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
            break;
        }
        case OBJECT_PROPERTY_TURNMASS:
        {
            float fTurnMass;
            argStream.ReadNumber(fTurnMass);
            if (CStaticFunctionDefinitions::SetObjectTurnMass(*pEntity, fTurnMass))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
            break;
        }
        case OBJECT_PROPERTY_AIRRESISTANCE:
        {
            float fAirResistance;
            argStream.ReadNumber(fAirResistance);
            if (CStaticFunctionDefinitions::SetObjectAirResistance(*pEntity, fAirResistance))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
            break;
        }
        case OBJECT_PROPERTY_ELASTICITY:
        {
            float fElasticity;
            argStream.ReadNumber(fElasticity);
            if (CStaticFunctionDefinitions::SetObjectElasticity(*pEntity, fElasticity))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
            break;
        }
        case OBJECT_PROPERTY_CENTEROFMASS:
        {
            CVector vecCenterOfMass;
            if (argStream.NextIsVector3D())
            {
                argStream.ReadVector3D(vecCenterOfMass);
            }
            else
            {
                argStream.ReadNumber(vecCenterOfMass.fX);
                argStream.ReadNumber(vecCenterOfMass.fY);
                argStream.ReadNumber(vecCenterOfMass.fZ);
            }
            if (CStaticFunctionDefinitions::SetObjectCenterOfMass(*pEntity, vecCenterOfMass))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
            break;
        }
        case OBJECT_PROPERTY_BUOYANCY:
        {
            float fBuoyancyConstant;
            argStream.ReadNumber(fBuoyancyConstant);
            if (CStaticFunctionDefinitions::SetObjectBuoyancyConstant(*pEntity, fBuoyancyConstant))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
            break;
        }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}
