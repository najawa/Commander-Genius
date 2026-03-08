/*
 * CSDLVideo.h
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 *
 *  This class
 */

#ifndef CSDLVIDEO_H_
#define CSDLVIDEO_H_

#include <base/video/CVideoEngine.h>
#include <graphics/GsSurface.h>

#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#endif

class CSDLVideo : public CVideoEngine
{
public:
	CSDLVideo(const CVidConfig& VidConfig);
    ~CSDLVideo() override;

    bool init();

    void resizeDisplayScreen(const GsRect<Uint16>& newDim);
	void collectSurfaces();
	void clearSurfaces();

    void setLightIntensity(const float intensity);

    void scaleNoFilter(	SDL_Surface *srcSfc,
                        const SDL_Rect *scrrect,
                        SDL_Surface *dstSfc,
                        const SDL_Rect *dstrect );


    bool initOverlaySurface( const Uint16 width,
                             const Uint16 height );

    void transformScreenToDisplay();

private:

    GsSurface mOverlaySurface;
};

#endif /* CSDLVIDEO_H_ */
