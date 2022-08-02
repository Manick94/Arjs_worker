/*
 *	gsub_es.h
 *  ARToolKit5
 *
 *	Graphics Subroutines (OpenGL ES 1.1) for ARToolKit.
 *
 *  This file is part of ARToolKit.
 *
 *  ARToolKit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ARToolKit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  As a special exception, the copyright holders of this library give you
 *  permission to link this library with independent modules to produce an
 *  executable, regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the terms and
 *  conditions of the license of that module. An independent module is a module
 *  which is neither derived from nor based on this library. If you modify this
 *  library, you may extend this exception to your version of the library, but you
 *  are not obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  Copyright 2015 Daqri, LLC.
 *  Copyright 2008-2015 ARToolworks, Inc.
 *
 *  Author(s): Philip Lamb
 *
 *	Rev		Date		Who		Changes
 *  1.0.0   2008-03-30  PRL     Rewrite of gsub_lite.c from ARToolKit v4.3.2 for OpenGL ES 1.1 capabilities.
 *
 */

/*!
	@header gsub_es
	@abstract A collection of useful OpenGL ES 1.1 routines for ARToolKit.
	@discussion
		gsub_es is the preferred means for drawing camera video
		images acquired from ARToolKit's video libraries. It includes
		optimized texture handling, and a variety of flexible drawing
		options.
 
		gsub_es also provides utility functions for setting the
		OpenGL viewing frustum and camera position based on ARToolKit-
		camera parameters and marker positions.
 
		gsub_es does not depend on GLUT, or indeed, any particular
		window or event handling system. It is therefore well suited
		to use in applications which have their own window and event
		handling code.
  
	@copyright 2008-2013 Philip Lamb
    @version 1.0.0
 */

#ifndef __gsub_es_h__
#define __gsub_es_h__

// ============================================================================
//	Public includes.
// ============================================================================

#if defined ANDROID
#  include <GLES/gl.h>
#  include <GLES/glext.h>
#else
#  include <OpenGLES/ES1/gl.h>
#  include <OpenGLES/ES1/glext.h>
#endif

#include <AR/config.h>
#include <AR/ar.h>		// ARUint8, AR_PIXEL_FORMAT
#include <AR/param.h>	// ARParam, arParamDecompMat(), arParamObserv2Ideal()
#include "glStateCache.h"

#ifdef __cplusplus
extern "C" {
#endif
    
// ============================================================================
//	Public types and definitions.
// ============================================================================

#define ARGL_DISABLE_DISP_IMAGE 0

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

#if !ARGL_DISABLE_DISP_IMAGE

/*!
    @typedef ARGL_CONTEXT_SETTINGS_REF
    @abstract Opaque type to hold ARGL settings for a given OpenGL context.
    @discussion
		An OpenGL context is an implementation-defined structure which
		keeps track of OpenGL state, including textures and display lists.
		Typically, individual OpenGL windows will have distinct OpenGL
		contexts assigned to them by the host operating system.
 
		As gsub_lite uses textures and display lists, it must be able to
		track which OpenGL context a given texture or display list it is using
		belongs to. This is especially important when gsub_lite is being used to
		draw into more than one window (and therefore more than one context.)
 
		Basically, functions which depend on OpenGL state, will require an
		ARGL_CONTEXT_SETTINGS_REF to be passed to them. An ARGL_CONTEXT_SETTINGS_REF
		is generated by setting the current OpenGL context (e.g. if using GLUT,
		you might call glutSetWindow()) and then calling arglSetupForCurrentContext().
		When you have finished using ARGL in a given context, you should call
		arglCleanup(), passing in an ARGL_CONTEXT_SETTINGS_REF, to free the
		memory used by the settings structure.
	@availability First appeared in ARToolKit 2.68.
 */
typedef struct _ARGL_CONTEXT_SETTINGS *ARGL_CONTEXT_SETTINGS_REF;

#endif

// ============================================================================
//	Public functions.
// ============================================================================
	
#if !ARGL_DISABLE_DISP_IMAGE

/*!
    @function
    @abstract Initialise the gsub_lite library for the current OpenGL context.
    @discussion
		This function performs required setup of the gsub_lite library
		for the current OpenGL context and must be called before any other argl*()
		functions are called for this context.
 
		An OpenGL context holds all of the state of the OpenGL machine, including
		textures and display lists etc. There will usually be one OpenGL context
		for each window displaying OpenGL content.
 
		Other argl*() functions whose operation depends on OpenGL state will
		require an ARGL_CONTEXT_SETTINGS_REF. This is just so that
		they can keep track of per-context variables.
 
		You should call arglCleanup() passing in the ARGL_CONTEXT_SETTINGS_REF
		when you have finished with the library for this context.
    @param cparam Pointer to a set of ARToolKit camera parameters for the
        current video source. The size of the source image is taken from the
        fields xsize and ysize of the ARParam structure pointed to. Also, when
        the draw mode is AR_DRAW_BY_TEXTURE_MAPPING (see the documentation for
        the global variable arglDrawMode) the field dist_factor of the ARParam
        structure pointed to will be taken as the amount to un-warp the supplied
        image.
 
        Prior to ARToolKit v5.0.5, the ARParam structure pointed to had to remain
        valid in memory for the duration of the ARGL_CONTEXT_SETTINGS_REF's usage.
        As of ARToolKit v5.0.5, the ARParam structure pointed to is copied, and
        no longer need be maintained.
    @param pixelFormat The type and format of pixels of the images to be drawn by the
        gsub library. This value can be changed later by using arglPixelFormatSet()
    @result An ARGL_CONTEXT_SETTINGS_REF. See the documentation for this type for more info.
	@availability First appeared in ARToolKit 2.68.
*/
ARGL_CONTEXT_SETTINGS_REF arglSetupForCurrentContext(ARParam *cparam, AR_PIXEL_FORMAT pixelFormat);
    
/*!
    @function
    @abstract Free memory used by gsub_lite associated with the specified context.
    @discussion
		Should be called after no more argl* functions are needed, in order
		to prevent memory leaks etc.
 
		The library can be setup again for the context at a later time by calling
		arglSetupForCurrentContext() again.
	@param contextSettings A reference to ARGL's settings for an OpenGL
		context, as returned by arglSetupForCurrentContext().
	@availability First appeared in ARToolKit 2.68.
*/
void arglCleanup(ARGL_CONTEXT_SETTINGS_REF contextSettings);

/*!
    @function
    @abstract Display an ARVideo image, by drawing it using OpenGL.
    @discussion
		This function draws an image from an ARVideo source to the current
		OpenGL context. This operation is most useful in video see-through
		augmented reality applications for drawing the camera view as a
		background image, but can also be used in other ways.
 
		An undistorted image is drawn with the lower-left corner of the
		bottom-left-most pixel at OpenGL screen coordinates (0,0), and the
		upper-right corner of the top-right-most pixel at OpenGL screen
		coodinates (x * zoom, y * zoom), where x and y are the values of the
		fields cparam->xsize and cparam->ysize (see below) and zoom is the
		value of the parameter zoom (also see below). If cparam->dist_factor
		indicates that an un-warping correction should be applied, the actual
		coordinates will differ from the values specified here. 
 
		OpenGL state: Drawing is performed with depth testing and lighting
		disabled, and thus leaves the the depth buffer (if any) unmodified. If
		pixel transfer is by texturing (see documentation for arglDrawMode),
		the drawing is done in replacement texture environment mode.
		The depth test enable and lighting enable state and the texture
		environment mode are restored before the function returns.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. It
		is the callers responsibility to make sure that the current context at the
		time arglDisplayImage() is called matches that under which contextSettings
		was created.
	@availability First appeared in ARToolKit 2.68.
*/
void arglDispImage(ARGL_CONTEXT_SETTINGS_REF contextSettings);

/*!
	@function
    @abstract Display an ARVideo image, by drawing it using OpenGL, using and modifying current OpenGL state.
    @discussion
		This function is identical to arglDispImage except that whereas
		arglDispImage sets an orthographic 2D projection and the OpenGL state
		prior to drawing, this function does not. It also does not restore any
		changes made to OpenGL state.
 
		This allows you to do effects with your image, other than just drawing it
		2D and with the lower-left corner of the bottom-left-most pixel attached
		to the bottom-left (0,0) of the window. For example, you might use a
		perspective projection instead of an orthographic projection with a
		glLoadIdentity() / glTranslate() on the modelview matrix to place the
		lower-left corner of the bottom-left-most pixel somewhere other than 0,0
		and leave depth-testing enabled.
 
		See the documentation for arglDispImage() for more information.
	@availability First appeared in ARToolKit 2.68.2.
 */
void arglDispImageStateful(ARGL_CONTEXT_SETTINGS_REF contextSettings);

/*!
    @function
    @abstract Set compensation for camera lens distortion in arglDispImage to off or on.
    @discussion
		By default, arglDispImage compensates for the distortion of the camera's
		acquired image caused by the lens when it draws. By calling this function
		with enabled = FALSE, this compensation will be disabled in the specified
		drawing context. It may be re-enabled at any time.
		This function is useful if you need to draw an image, but do not know the
		extent of the camera's lens distortion (such as during distortion calibration).
		While distortion compensation is disabled, the dist_factor[] array in a
		the camera cparam structure passed to arglDispImage is ignored.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. 
	@param enable TRUE to enabled distortion compensation, FALSE to disable it.
		The default state for new contexts is enable = TRUE.
	@result TRUE if the distortion value was set, FALSE if an error occurred.
	@availability First appeared in ARToolKit 2.71.
*/
char arglDistortionCompensationSet(ARGL_CONTEXT_SETTINGS_REF contextSettings, char enable);

/*!
    @function
	@abstract Enquire as to the enable state of camera lens distortion compensation in arglDispImage.
	@discussion
		By default, arglDispImage compensates for the distortion of the camera's
		acquired image caused by the lens when it draws. This function enquires
		as to whether arglDispImage is currently doing compensation or not.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. 
	@param enable Pointer to an integer value which will be set to TRUE if distortion
		compensation is enabled in the specified context, or FALSE if it is disabled.
	@result TRUE if the distortion value was retreived, FALSE if an error occurred.
	@availability First appeared in ARToolKit 2.71.
 */
char arglDistortionCompensationGet(ARGL_CONTEXT_SETTINGS_REF contextSettings, char *enable);

/*!
    @function
    @abstract Set the current video image drawing scalefactor.
    @param zoom The amount to scale the video image up or down. To draw the video
        image double size, use a zoom value of 2.0. To draw the video image
        half size use a zoom value of 0.5.
 */
char arglSetPixelZoom(ARGL_CONTEXT_SETTINGS_REF contextSettings, float zoom);

/*!
    @function
    @abstract Retrieve the current video image drawing scalefactor.
    @param zoom The amount to scale the video image up or down. To draw the video
        image double size, use a zoom value of 2.0. To draw the video image
        half size use a zoom value of 0.5.
 */
char arglGetPixelZoom(ARGL_CONTEXT_SETTINGS_REF contextSettings, float *zoom);
    
/*!
    @function
    @abstract Set the format of pixel data which will be passed to arglDispImage*()
    @discussion (description)
		In gsub_lite, the format of the pixels (i.e. the arrangement of components
		within each pixel) can be changed at runtime. Use this function to inform
		gsub_lite the format the pixels being passed to arglDispImage*() functions
		are in. This setting applies only to the context passed in parameter
		contextSettings. The default format is determined by
		the value of AR_DEFAULT_PIXEL_FORMAT at the time the library was built.
		Usually, image data is passed in directly from images generated by ARVideo,
		and so you should ensure that ARVideo is generating pixels of the same format.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. 
    @param format A symbolic constant for the pixel format being set. See
		AR_PIXEL_FORMAT in ar.h for a list of all possible formats.
	@result TRUE if the pixel format value was set, FALSE if an error occurred.
	@availability First appeared in ARToolKit 2.71.
*/
char arglPixelFormatSet(ARGL_CONTEXT_SETTINGS_REF contextSettings, AR_PIXEL_FORMAT format);

/*!
    @function
    @abstract Get the format of pixel data in which arglDispImage*() is expecting data to be passed.
    @discussion This function enquires as to the current format of pixel data being
		expected by the arglDispImage*() functions. The default format is determined by
		the value of AR_DEFAULT_PIXEL_FORMAT at the time the library was built.
	@param contextSettings A reference to ARGL's settings for the current OpenGL
		context, as returned by arglSetupForCurrentContext() for this context. 
	@param format A symbolic constant for the pixel format in use. See
		AR_PIXEL_FORMAT in ar.h for a list of all possible formats.
	@param size The number of bytes of memory occupied per pixel, for the given format.
	@result TRUE if the pixel format and size values were retreived, FALSE if an error occurred.
	@availability First appeared in ARToolKit 2.71.
*/
char arglPixelFormatGet(ARGL_CONTEXT_SETTINGS_REF contextSettings, AR_PIXEL_FORMAT *format, int *size);

/*!
    @function
    @abstract   Find out whether ARGL is rotating all OpenGL drawing by 90 degrees.
    @discussion
        On some OpenGL ES devices, it may be desirable to rotate all OpenGL drawing by 90
        degrees in the window coordinate system, in order to swap the horizontal and
        vertical axes of the device. This may be a higher performance solution than
        manually swapping rows and columns of submitted data.
 
        This function queries the current state of whether such a rotation is being performed by ARGL or not.
    @param contextSettings A reference to ARGL's settings for the current OpenGL
        context, as returned by arglSetupForCurrentContext() for this context.
    @result    TRUE if a 90 degree rotation is enabled, FALSE if it is disabled.
*/
char arglGetRotate90(ARGL_CONTEXT_SETTINGS_REF contextSettings);

/*!
    @function
    @abstract   Set or unset a 90 degree rotation in all OpenGL drawing performed by ARGL.
    @discussion
        On some OpenGL ES devices, it may be desirable to rotate all OpenGL drawing by 90
        degrees in the window coordinate system, in order to swap the horizontal and
        vertical axes of the device. This may be a higher performance solution than
        manually swapping rows and columns of submitted data.
 
        This function enables or disables such an axis swap in ARGL with very little 
        performance cost, by introducing a 90-degree rotation into the OpenGL projection matrix.
 
        By default, 90 degree rotation is DISABLED.
    @param contextSettings A reference to ARGL's settings for the current OpenGL.
        context, as returned by arglSetupForCurrentContext() for this context.
    @param rotate90 Set to TRUE or 1 to enable 90 degree rotation, FALSE to disable.
*/
void arglSetRotate90(ARGL_CONTEXT_SETTINGS_REF contextSettings, char rotate90);

char arglGetFlipH(ARGL_CONTEXT_SETTINGS_REF contextSettings);
void arglSetFlipH(ARGL_CONTEXT_SETTINGS_REF contextSettings, char flipH);
char arglGetFlipV(ARGL_CONTEXT_SETTINGS_REF contextSettings);
void arglSetFlipV(ARGL_CONTEXT_SETTINGS_REF contextSettings, char flipV);

/*!
    @function
    @abstract   Specify a desired pixel buffer size larger than the camera image size.
    @discussion
        By default, the pixel buffer accepted by function arglPixelBufferDataUpload()
        is assumed to be tightly packed, row-major array of dimensions
        equal to the calibrated camera image size (as passed in the fields arParam.xsize and arParam.ysize
        of the ARHandle submitted to arglSetupForCurrentContext().

        The pixel data buffer submitted may, under some circumstances be allowed to be larger than the
        values of the calibrated camera image size (i.e. padded). This may only occur when the
        underlying OpenGL ES impementation does not support non-power-of-two textures. For the Apple
        iPhone family, this applies to the iPhone, the iPhone 3G, the iPod Touch 1st and 2nd Generation.
        The iPhone 3GS and the iPod Touch 3rd Generation support non-power-of-two textures, and thus
        padding of the pixel data buffer is not supported on these devices. If padding is desired,
        the desired buffer size must be submitted using this function. Check that the
        result from this function is TRUE before attempting to upload such a padded buffer.
    @param contextSettings A reference to ARGL's settings for the current OpenGL
        context, as returned by arglSetupForCurrentContext() for this context.
    @param bufWidth The desired buffer width, in pixels.
    @param bufHeight The desired buffer height, in pixels.
    @result TRUE if the desired buffer size is supported, FALSE otherwise.
*/
char arglPixelBufferSizeSet(ARGL_CONTEXT_SETTINGS_REF contextSettings, int bufWidth, int bufHeight);

/*!
    @function
    @abstract   Query the size of pixel data buffers expected by arglPixelBufferDataUpload().
    @discussion
        See function arglPixelBufferSizeSet() for a full discussion.
    @param contextSettings A reference to ARGL's settings for the current OpenGL
        context, as returned by arglSetupForCurrentContext() for this context.
    @param bufWidth A pointer to int, which will be filled out with the buffer width, in pixels, or NULL if this value is not required.
    @param bufHeight A pointer to int, which will be filled out with the buffer height, in pixels, or NULL if this value is not required..
    @result TRUE if the buffer size was successfully queried, FALSE otherwise.
 */
char arglPixelBufferSizeGet(ARGL_CONTEXT_SETTINGS_REF contextSettings, int *bufWidth, int *bufHeight);

/*!
    @function
    @abstract   Upload a buffer of pixel data to an OpenGL texture for later use by arglDispImage().
    @discussion
        ARGL provides the ability to perform distortion-compensated texturing of a camera video image
        into the OpenGL context, for the purpose of video-see through augmented reality. This function
        uploads the camera image data to an OpenGL texture, ready for later drawing using the function
        arglDispImage().
    @param contextSettings A reference to ARGL's settings for the current OpenGL
        context, as returned by arglSetupForCurrentContext() for this context.
    @param bufDataPtr0 A pointer to the pixel buffer, which is a block of memory from which texture
        data will be read.
        
        The layout of pixel data in the memory pointed to by bufDataPtr is assumed to be
        specified by the value of pixelFormat in the ARHandle submitted to arglSetupForCurrentContext(),
        but can be changed by calling arglPixelFormatSet() and/or arglPixelBufferSizeSet().
 
        By default, the pixel buffer is assumed to be tightly packed, row-major array of dimensions
        equal to the calibrated camera image size (as passed in the fields arParam.xsize and arParam.ysize
        of the ARHandle submitted to arglSetupForCurrentContext().
 
        The pixel data buffer submitted may, under some circumstances be allowed to be larger than the
        values of the calibrated camera image size (i.e. padded). See the discussion section of the
        documentation for function arglPixelBufferSizeSet() for more information.
 
        This may only occur when the
        underlying OpenGL ES impementation does not support non-power-of-two textures. For the Apple
        iPhone family, this applies to the iPhone, the iPhone 3G, the iPod Touch 1st and 2nd Generation.
        The iPhone 3GS and the iPod Touch 3rd Generation support non-power-of-two textures, and thus
        padding of the pixel data buffer is not supported on these devices. If padding is desired,
        the desired buffer size must be submitted using arglPixelBufferSizeSet(). Check that the
        result from this function is TRUE before attempting to upload such a padded buffer.
    @param bufDataPtr1 NULL for single-plane (interleaved) textures, otherwise for bi-planar textures,
        a pointer to the pixel buffer, which is a block of memory from which texture
        data will be read for the second plane containing the CbCr components. This plane must have
        dimensions of half the size of plane 0, and 2-bytes per pixel (1 byte Cb, 1 byte Cr).
    @result TRUE if the pixel buffer was successfully uploaded to OpenGL, FALSE otherwise.
*/
char arglPixelBufferDataUploadBiPlanar(ARGL_CONTEXT_SETTINGS_REF contextSettings, ARUint8 *bufDataPtr0, ARUint8 *bufDataPtr1);
#define arglPixelBufferDataUpload(contextSettings,bufDataPtr) arglPixelBufferDataUploadBiPlanar(contextSettings,bufDataPtr,NULL)
    
#endif // !ARGL_DISABLE_DISP_IMAGE

/*!
    @function
    @abstract   Check for the availability of an OpenGL extension.
    @discussion
        Provides the same functionality as the gluCheckExtension() function,
        since some platforms don't have GLU version 1.3 or later.
    @param      extName Name of the extension, e.g. "GL_EXT_texture".
    @param      extString The OpenGL extensions string, as returned by glGetString(GL_EXTENSIONS);
    @result     TRUE, if the extension is found, FALSE otherwise.
*/
GLboolean arglGluCheckExtension(const GLubyte* extName, const GLubyte *extString);

/*!
    @function
    @abstract   Checks for the presence of an OpenGL capability by version or extension.
    @discussion
        Checks for the presence of an OpenGL capability by version or extension.
        The test returns true if EITHER the OpenGL driver's OpenGL implementation
        version meets or exceeds a minimum value (passed in in minVersion) OR
        if an OpenGL extension identifier passed in as a character string
        is non-NULL, and is found in the current driver's list of supported extensions.
    @param      minVersion
        A binary-coded decimal (i.e. version 1.0 is represented as 0x0100) version number.
        If minVersion is zero, (i.e. there is no version of OpenGL with this extension in core)
        the version test will always fail, and the result will only be true if the extension
        string test passes.
    @param      extension A string with an extension name to search the drivers extensions
        string for. E.g. "GL_EXT_texture". If NULL, the extension name test will always fail,
        and the result will only be true if the version number test passes.
    @result     TRUE If either of the tests passes, or FALSE if both fail.
*/
int arglGLCapabilityCheck(const unsigned short minVersion, const unsigned char *extension);

#ifdef __cplusplus
}
#endif
#endif /* !__gsub_es_h__ */
