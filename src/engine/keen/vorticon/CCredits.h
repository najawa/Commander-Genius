/*
 * CCredits.h
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#ifndef CCREDITS_H_
#define CCREDITS_H_

#include <vector>
#include <string>
#include <memory>
#include "CInfoScene.h"
#include "engine/core/CMap.h"
#include <graphics/GsFont.h>
#include <array>

class CCredits : public CInfoScene
{
public:
	
	void init() override;
    void ponder() override;
    void render() override;
	void teardown() override;
	
private:

    std::array<std::string, 56> m_scrolltext;
    std::array<int, 56> m_mid;
	int m_timer;
	int m_scrolly;
	
	std::shared_ptr<CMap> mpMap;
    GsSurface mDrawSfc;
};

#endif /* CCREDITS_H_ */
