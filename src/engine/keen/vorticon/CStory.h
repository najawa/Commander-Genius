/*
 * CStory.h
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 */

#ifndef CSTORY_H_
#define CSTORY_H_

#include "CInfoScene.h"
#include "engine/core/CMap.h"
#include "dialog/CTextViewer.h"
#include "fileio/CExeFile.h"
#include <memory>
#include <string.h>


class CStory : public CInfoScene
{
public:

	void init() override;
    void ponder() override;
    void render() override;
	void teardown() override;

private:
	std::shared_ptr<CMap> mpMap;
	std::unique_ptr<CTextViewer> mpTextViewer;
};

#endif /* CSTORY_H_ */
