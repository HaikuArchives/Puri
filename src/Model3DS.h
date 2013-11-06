/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef MODEL3DS_H
#define MODEL3DS_H

#include <cstdio>
#include <string>

#include <String.h>
#include <Point.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <lib3ds.h>
#include <png.h>


#include "Tuple.h"



using namespace std;

#define TEXTURE_LOAD_ERROR 0

class Model3DS
{
public:
                    Model3DS(Model3DS & model);
                    Model3DS(void);
                    Model3DS(BString const& kFilename, bool showTexture = false);
                    Model3DS(int const& kProperty);

    virtual void    Draw();// const;
    virtual         ~Model3DS();

    void Load(BString const& kFilename);

    void Load(int const& kProperty)
    {
        fProperty = kProperty;

        Load( GetPath( kProperty ) );
    }

    void SetPosition(float const& kPosX, float const& kPosY)
    {
        fPosX = kPosX;
        fPosY = kPosY;
    }

    void SetDeltaPos(float const& kDeltaX, float const& kDeltaY);

    void SetHighlight(bool const& highlight)
    {
        fHighlight = highlight;
    }

    inline  void SetChild(bool isChild = true)
    {
        fIsChild = isChild;
    }

    inline void SetPosition(BPoint const& kPos)
    {
        SetPosition(kPos.x, kPos.y);
    }

    inline void SetCoordinate(float const& kCoordinateX,
                                                      float const& kCoordinateY)
    {
        SetPosition(kCoordinateX*2 - 7, kCoordinateY*2 -7);
    }

    inline void SetCoordinate(Tuple<uint> const& kCoordinate)
    {
        SetCoordinate(kCoordinate.x, kCoordinate.y);
    }

    inline void SetVisible(bool visible)
    {
        fVisible = visible;
    }

    inline void EnableTexture(bool textureIsOn)
    {
        fTextureIsOn = textureIsOn;
    }


    inline bool IsFlag(int const& kFlag) const
    {
        return ((fProperty & kFlag) == kFlag);
    }

    inline bool IsChild(void)
    {
        return fIsChild;
    }

    bool const& IsVisible(void)
    {
        return fVisible;
    }

    inline uint ID(void)
    {
        return (GetCoordinate().y*8 + GetCoordinate().x);
    }

    Tuple<uint> GetCoordinate(void)
    {
        return Tuple<uint>((fPosX + 7)/2, (fPosY + 7)/2);
    }

    BString GetPath(int const& kProperty);

protected:
    GLuint _LoadTexture(BString const& filename, int &width, int &height);
    GLuint _LoadRawTexture(BString const& filename, int  width, int  height);

    GLfloat	        (*fNormals)[3];

    Lib3dsFile*		fModel;

    GLuint			fTexture;
    float	        fPosX;
    float	        fPosY;
    float	        fPosZ;
    int		        fProperty;
    bool        	fHighlight;
    bool	        fVisible;
    bool	        fTextureIsOn;
    bool            fIsChild;
    uint            fID;
};


#endif
