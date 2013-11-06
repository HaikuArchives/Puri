/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "Model3DS.h"

#include <iostream>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

#include "BackPiece.h"
#include "ModelPath.h"
#include "PngLoad.h"
#include "Tools.h"

using namespace std;


Model3DS::Model3DS(void)
	:
    fPosX(0),
    fPosY(0),
    fPosZ(0),
    fProperty(0),
    fHighlight(false),
    fTextureIsOn(false),
    fIsChild(false),
    fID(0)
{
        fNormals = NULL;
        fModel   = NULL;

}


Model3DS::Model3DS(Model3DS & model)
    :
    fPosX(0),
    fPosY(0),
    fPosZ(0),
    fProperty(0),
    fHighlight(false),
    fTextureIsOn(false),
    fIsChild(true),
    fID(0)
{
  fModel = model.fModel;
  fNormals = model.fNormals;
}


// Load 3DS model
Model3DS::Model3DS(BString const& filename, bool showTexture)
	:
    fPosX(0),
    fPosY(0),
    fPosZ(0),
    fProperty(0),
    fHighlight(false),
    fTextureIsOn(false),
    fIsChild(false),
    fID(0)
{
	fNormals	= NULL;
	fModel		= NULL;

	Load(filename);

	if (showTexture) {
		fTextureIsOn = true;
		fTexture = loadPNGTexture( GetAppPath().Append(
                               "/data/Texture/Board/board.png" ).String() );
	}
}


Model3DS::Model3DS(int const& kProperty)
    :
    fPosX(0),
    fPosY(0),
    fPosZ(0),
    fProperty(kProperty),
    fHighlight(false),
    fTextureIsOn(false),
    fIsChild(false),
    fID(0)
{
	fNormals	= NULL;
	fModel		= NULL;

	Load(GetPath(kProperty));
}


BString
Model3DS::GetPath(int const& kProperty)
{
	BString path;

	switch (kProperty) {
		case K_W:
			path = GetAppPath().Append(KING_W_PATH);
			break;

        case K_B:
			path = GetAppPath().Append(KING_B_PATH);
			break;

		case Q_W:
			path = GetAppPath().Append(QUEEN_W_PATH);
			break;

        case Q_B:
			path = GetAppPath().Append(QUEEN_B_PATH);
			break;

		case R_W:
			path = GetAppPath().Append(ROOK_W_PATH);
			break;

		case R_B:
			path = GetAppPath().Append(ROOK_B_PATH);
			break;

		case B_W:
			path = GetAppPath().Append(BISHOP_W_PATH);
			break;

		case B_B:
			path = GetAppPath().Append(BISHOP_B_PATH);
			break;

		case N_W:
			path = GetAppPath().Append(KNIGHT_W_PATH);
			break;

		case N_B:
			path = GetAppPath().Append(KNIGHT_B_PATH);
			break;

		case P_W:
			path = GetAppPath().Append(PAWN_W_PATH);
			break;

		case P_B:
			path = GetAppPath().Append(PAWN_B_PATH);
			break;

		default:
			path = BString();
			break;
	}

	return path;
}


void
Model3DS::Load(BString const& filename)
{
	if (fModel != NULL) {
		delete[] fNormals;
		fNormals = NULL;
		lib3ds_file_free(fModel);
		fModel = NULL;
		glDeleteTextures (1, &fTexture);
	}

	fModel = lib3ds_file_open(filename.String());

	// If loading the model failed, we throw an exception
	if (fModel == NULL)
        return;
		//throw BString( "Opening the file not possible" );


	fNormals = new GLfloat[ (*fModel->meshes)->nfaces][3];
	lib3ds_mesh_calculate_face_normals( *fModel->meshes, fNormals);

	//load texture-here to use
}


// Destructor
Model3DS::~Model3DS()
{
    if (fIsChild == false) {
        delete[] fNormals;
        lib3ds_file_free(fModel);
        glDeleteTextures (1, &fTexture);
    }

    //delete[] fVertices;
	//delete[] fFaces;
}


// TODO: Render the model using Vertex Buffer Objects
void Model3DS::Draw() //const
{
	glPushMatrix();
	glTranslatef( fPosX, fPosY, fPosZ );

    //create the list of required normals

    glBegin(GL_TRIANGLES);

    for (unsigned long i = 0; i < (*fModel->meshes)->nfaces; i++) {
        if (fHighlight) {
            GLfloat ambient[4]={0.0, 0.0, 1.0, 1.0};
            glMaterialf (GL_FRONT, GL_SHININESS, 128);
            glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE,fModel->
                    materials[(*fModel->meshes)->faces[i].material]->diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, fModel->
                    materials[(*fModel->meshes)->faces[i].material ]->specular);
        } else {
            //set material
            //glMaterialf (GL_FRONT, GL_SHININESS, fModel->materials[ (*fModel->meshes)->faces[i].material ]->shininess);
            glMaterialf (GL_FRONT, GL_SHININESS, 128);
            glMaterialfv(GL_FRONT, GL_AMBIENT  , fModel->
                    materials[ (*fModel->meshes)->faces[i].material ]->ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE,fModel->
                    materials[ (*fModel->meshes)->faces[i].material ]->diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, fModel->
                   materials[ (*fModel->meshes)->faces[i].material ]->specular);
        }
        //set Face Normal
        glNormal3fv(fNormals[i]);
        glVertex3fv((*fModel->meshes)->
                                vertices[(*fModel->meshes)->faces[i].index[0]]);
        glVertex3fv((*fModel->meshes)->
                                vertices[(*fModel->meshes)->faces[i].index[1]]);
        glVertex3fv((*fModel->meshes)->
                                vertices[(*fModel->meshes)->faces[i].index[2]]);
    }

    glEnd();

    if (fTextureIsOn) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, fTexture);
            glBegin(GL_QUADS);
                glNormal3f(0.0, 0.0, 1.0);
                glTexCoord2d(0, 0); glVertex3f(-8.6, +8.6, 0.1);
                glTexCoord2d(1, 0); glVertex3f( 8.6, +8.6, 0.1);
                glTexCoord2d(1, 1); glVertex3f( 8.6, -8.6, 0.1);
                glTexCoord2d(0, 1); glVertex3f(-8.6, -8.6, 0.1);
            glEnd();
        glDisable(GL_TEXTURE_2D);
    }

	glPopMatrix();
}


void Model3DS::SetDeltaPos(float const& kDeltaX, float const& kDeltaY)
{
	fPosX += kDeltaX;
	fPosY += kDeltaY;

	if (fPosX < -7)	    fPosX = -7;
	if (fPosY < -7)	    fPosY = -7;
	if (fPosX >  7)	    fPosX =  7;
	if (fPosY >  7)	    fPosY =  7;

}


GLuint Model3DS::_LoadTexture(BString const& filename, int &width, int &height)
{
   // header for testing if it is a png
   png_byte header[8];

   // open file as binary
   FILE *fp = fopen(filename.String(), "rb");
   if (!fp) {
     return TEXTURE_LOAD_ERROR;
   }

   // read the header
   fread(header, 1, 8, fp);

   // test if png
   int is_png = !png_sig_cmp(header, 0, 8);
   if (!is_png) {
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   // create png struct
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                                    NULL, NULL);

   if (!png_ptr) {
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   // create png info struct
   png_infop info_ptr = png_create_info_struct(png_ptr);

   if (!info_ptr) {
     png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   // create png info struct
   png_infop end_info = png_create_info_struct(png_ptr);
   if (!end_info) {
     png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   if (setjmp(png_jmpbuf(png_ptr))) {
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   // init png reading
   png_init_io(png_ptr, fp);

   //let libpng know you already read the first 8 bytes
   png_set_sig_bytes(png_ptr, 8);

   // read all the info up to the image data
   png_read_info(png_ptr, info_ptr);

   //variables to pass to get info
   int bit_depth, color_type;
   png_uint_32 twidth, theight;

   // get info about png
   png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
       NULL, NULL, NULL);

   //update width and height based on png info
   width = twidth;
   height = theight;

   // Update the png info struct.
   png_read_update_info(png_ptr, info_ptr);

   // Row size in bytes.
   int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

   // Allocate the image_data as a big block, to be given to opengl
   png_byte *image_data = new png_byte[rowbytes * height];
   if (!image_data) {
     // clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   //row_pointers is for pointing to image_data for reading the png with libpng
   png_bytep *row_pointers = new png_bytep[height];
   if (!row_pointers) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     delete[] image_data;
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
   // set the individual row_pointers to point at the correct offsets of image_data
   for (int i = 0; i < height; ++i)
     row_pointers[height - 1 - i] = image_data + i * rowbytes;

   //read the png into image_data through row_pointers
   png_read_image(png_ptr, row_pointers);

   //Now generate the OpenGL texture object
   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0,
       GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   //clean up memory and close stuff
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
   delete[] image_data;
   delete[] row_pointers;
   fclose(fp);

   return texture;
}


typedef uchar BYTE;

GLuint
Model3DS::_LoadRawTexture(BString const& filename, int width, int height)
{
	int wrap = 0;

    GLuint  texture;
    BYTE*   data;
    FILE*   file;

    // open texture data
    file = fopen(filename, "rb");
    if (file == NULL) return 0;

    data = (BYTE*) malloc(width * height * 3);

    // read texture data
    fread(data, width * height * 3, 1, file);
    fclose(file);

    // allocate a texture name
    glGenTextures(1, &texture);

    // select our current texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                                      GL_LINEAR_MIPMAP_NEAREST);
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                                   wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                                   wrap ? GL_REPEAT : GL_CLAMP);

    // build our texture mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height,
                                                GL_RGB, GL_UNSIGNED_BYTE, data);

    // free buffer
    free(data);

    return texture;
}


/*	Using VBOs doesn't work yet
// Render the model using Vertex Buffer Objects
void Model3DS :: Draw() const
{
	// Enable vertex and normal arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Bind the vbo with the normals
	glBindBuffer(GL_ARRAY_BUFFER, fNormalVBO);
	// The pointer for the normals is NULL which means that OpenGL will use the currently bound vbo
	glNormalPointer(GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, fVertexVBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	// Render the triangles
	glDrawArrays(GL_TRIANGLES, 0, fTotalFaces * 3);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}
*/

/*
// Load 3DS model
Model3DS :: Model3DS( std::string const& filename )
	: fPosX( 0 ), fPosY( 0 ), fPosZ( 0 )
{
	Lib3dsFile*	model = lib3ds_file_open( filename.c_str() );

	// If loading the model failed, we throw an exception
	if (model == NULL)
		throw string( "Opening the file not possible" );

	if (model->nmeshes != 1)
		throw string( "The Loaded Model has not exactly one Mesh" );

	// Loop through every mesh
	Lib3dsMesh* mesh = *model->meshes;

	float (*orig_vertices)[3] = (float(*)[3])malloc(sizeof(float) * 3 * mesh->nvertices);
    memcpy(orig_vertices, mesh->vertices, sizeof(float) * 3 * mesh->nvertices);

    float (*normals)[3] = (float(*)[3])malloc(sizeof(float) * 9 * mesh->nfaces);
        lib3ds_mesh_calculate_vertex_normals(mesh, normals);

	// Generate a Vertex Buffer Object and store it with our vertices
	glGenBuffers(1, &fVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * 3 * fTotalFaces, orig_vertices, GL_STATIC_DRAW);

	// Generate another Vertex Buffer Object and store the normals in it
	glGenBuffers(1, &fNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * 3 * fTotalFaces, normals, GL_STATIC_DRAW);


	lib3ds_file_free( model );

	model = NULL;

}
*/

