#ifndef VORTICONENGINE_H
#define VORTICONENGINE_H

#include "engine/keen/KeenEngine.h"
#include "CEGAGraphicsVort.h"
#include "CInfoScene.h"

namespace vorticon
{


bool setupAudio();

class VorticonEngine : public KeenEngine
{
public:
    const int mEp;

    VorticonEngine(const bool openedGamePlay,
                   const int ep,
                   const std::string &dataPath);

    void ponder(const float deltaT) override;

    void render() override;

    void openMainMenu() override;

    bool loadResources( const Uint8 flags ) override;

    void switchToPassiveMode();

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr) override;

private:
    std::shared_ptr<CInfoScene> mpInfoScene;
};

}

// Events
struct StartVorticonEngine : public SwitchEngineEvent
{
    StartVorticonEngine(const bool openedGamePlay,
                        const int ep,
                        const std::string &dataPath) :
        SwitchEngineEvent( new vorticon::VorticonEngine(openedGamePlay, ep, dataPath) )
        { }
};


#endif /* VORTICONENGINE_H */
