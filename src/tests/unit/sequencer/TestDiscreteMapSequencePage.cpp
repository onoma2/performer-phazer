#include "UnitTest.h"

#include "apps/sequencer/ui/pages/DiscreteMapSequencePage.h"
#include "apps/sequencer/model/DiscreteMapSequence.h"
#include "apps/sequencer/model/Project.h"
#include "apps/sequencer/model/Model.h"
#include "engine/Engine.h"
#include "ui/MessageManager.h"
#include "ui/KeyState.h"

// Mock classes for testing
class MockEngine : public Engine {
public:
    MockEngine() : Engine() {}
    
    void writeRouted(Routing::Target target, int trackIndex, int intValue, float floatValue) override {}
    void writeRouted(Routing::Target target, int trackIndex, float floatValue) override {}
    void writeRouted(Routing::Target target, int trackIndex, bool value) override {}
    void writeRouted(Routing::Target target, int trackIndex, int value) override {}
    
    void triggerRise(int trackIndex) override {}
    void triggerFall(int trackIndex) override {}
    void setGate(int trackIndex, bool gate) override {}
    void setVoltage(int trackIndex, float voltage) override {}
    void setTrigger(int trackIndex) override {}
    
    void setPattern(int pattern) override {}
    void setRunMode(RunMode runMode) override {}
    void setRun(bool run) override {}
    
    void reset() override {}
    void resetTrack(int trackIndex) override {}
    void resetPattern(int pattern) override {}
    
    void tick(uint32_t tick) override {}
    void updateLeds(Leds &leds) override {}
    
    void setScale(int scale) override {}
    void setRootNote(int rootNote) override {}
    
    void setBpm(float bpm) override {}
    void setBpb(int bpb) override {}
    void setSwing(int swing) override {}
    
    void setLinkParameter(int trackIndex, int parameter, int value) override {}
    
    void setLinkSource(int trackIndex, int source) override {}
    void setLinkDestination(int trackIndex, int destination) override {}
    
    void setLinkEnabled(int trackIndex, bool enabled) override {}
    
    void setLinkValue(int trackIndex, int parameter, float value) override {}
    
    void setLinkModulation(int trackIndex, int parameter, float modulation) override {}
    
    void setLinkTarget(int trackIndex, int parameter, int target) override {}
    
    void setLinkSourceValue(int trackIndex, int source, float value) override {}
    
    void setLinkSourceModulation(int trackIndex, int source, float modulation) override {}
    
    void setLinkSourceTarget(int trackIndex, int source, int target) override {}
    
    void setLinkSourceEnabled(int trackIndex, int source, bool enabled) override {}
    
    void setLinkSourceParameter(int trackIndex, int source, int parameter) override {}
    
    void setLinkSourceModulationTarget(int trackIndex, int source, int target) override {}
    
    void setLinkSourceModulationValue(int trackIndex, int source, float value) override {}
    
    void setLinkSourceModulationEnabled(int trackIndex, int source, bool enabled) override {}
    
    void setLinkSourceModulationParameter(int trackIndex, int source, int parameter) override {}
    
    void setLinkSourceModulationSource(int trackIndex, int source, int modulationSource) override {}
    
    void setLinkSourceModulationAmount(int trackIndex, int source, float amount) override {}
    
    void setLinkSourceModulationPolarity(int trackIndex, int source, bool polarity) override {}
    
    void setLinkSourceModulationUnipolar(int trackIndex, int source, bool unipolar) override {}
    
    void setLinkSourceModulationBipolar(int trackIndex, int source, bool bipolar) override {}
    
    void setLinkSourceModulationInvert(int trackIndex, int source, bool invert) override {}
    
    void setLinkSourceModulationScale(int trackIndex, int source, float scale) override {}
    
    void setLinkSourceModulationOffset(int trackIndex, int source, float offset) override {}
    
    void setLinkSourceModulationSmooth(int trackIndex, int source, bool smooth) override {}
    
    void setLinkSourceModulationShape(int trackIndex, int source, int shape) override {}
    
    void setLinkSourceModulationPhase(int trackIndex, int source, float phase) override {}
    
    void setLinkSourceModulationRate(int trackIndex, int source, float rate) override {}
    
    void setLinkSourceModulationDepth(int trackIndex, int source, float depth) override {}
    
    void setLinkSourceModulationSync(int trackIndex, int source, bool sync) override {}
    
    void setLinkSourceModulationClockDiv(int trackIndex, int source, int clockDiv) override {}
    
    void setLinkSourceModulationPhaseReset(int trackIndex, int source, bool reset) override {}
    
    void setLinkSourceModulationPhaseResetSync(int trackIndex, int source, bool sync) override {}
    
    void setLinkSourceModulationPhaseResetClockDiv(int trackIndex, int source, int clockDiv) override {}
    
    void setLinkSourceModulationPhaseResetEnabled(int trackIndex, int source, bool enabled) override {}
    
    void setLinkSourceModulationPhaseResetParameter(int trackIndex, int source, int parameter) override {}
    
    void setLinkSourceModulationPhaseResetSource(int trackIndex, int source, int modulationSource) override {}
    
    void setLinkSourceModulationPhaseResetAmount(int trackIndex, int source, float amount) override {}
    
    void setLinkSourceModulationPhaseResetPolarity(int trackIndex, int source, bool polarity) override {}
    
    void setLinkSourceModulationPhaseResetUnipolar(int trackIndex, int source, bool unipolar) override {}
    
    void setLinkSourceModulationPhaseResetBipolar(int trackIndex, int source, bool bipolar) override {}
    
    void setLinkSourceModulationPhaseResetInvert(int trackIndex, int source, bool invert) override {}
    
    void setLinkSourceModulationPhaseResetScale(int trackIndex, int source, float scale) override {}
    
    void setLinkSourceModulationPhaseResetOffset(int trackIndex, int source, float offset) override {}
    
    void setLinkSourceModulationPhaseResetSmooth(int trackIndex, int source, bool smooth) override {}
    
    void setLinkSourceModulationPhaseResetShape(int trackIndex, int source, int shape) override {}
    
    void setLinkSourceModulationPhaseResetRate(int trackIndex, int source, float rate) override {}
    
    void setLinkSourceModulationPhaseResetDepth(int trackIndex, int source, float depth) override {}
    
    void setLinkSourceModulationPhaseResetSync(int trackIndex, int source, bool sync) override {}
    
    void setLinkSourceModulationPhaseResetClockDiv(int trackIndex, int source, int clockDiv) override {}
    
    TrackEngine &trackEngine(int trackIndex) override {
        // Return a dummy track engine for testing
        static TrackEngine dummyEngine;
        return dummyEngine;
    }
    
    void setTrackEngine(int trackIndex, std::unique_ptr<TrackEngine> engine) override {}
    
    void setTrackEngineParameter(int trackIndex, int parameter, int value) override {}
    void setTrackEngineParameter(int trackIndex, int parameter, float value) override {}
    void setTrackEngineParameter(int trackIndex, int parameter, bool value) override {}
    
    void setTrackEngineParameterModulation(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulation(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrackEngineParameterModulationPhaseResetParameter(int trackIndex, int parameter, int source, int parameter2) override {}
    void setTrackEngineParameterModulationPhaseResetSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationPhaseResetAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationPhaseResetPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationPhaseResetUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationPhaseResetBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationPhaseResetInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationPhaseResetScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationPhaseResetOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationPhaseResetSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationPhaseResetShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhaseResetRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationPhaseResetDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationSource(int trackIndex, int parameter, int source, int modulationSource) override {}
    void setTrackEngineParameterModulationAmount(int trackIndex, int parameter, int source, float amount) override {}
    void setTrackEngineParameterModulationEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    
    void setTrackEngineParameterModulationPolarity(int trackIndex, int parameter, int source, bool polarity) override {}
    void setTrackEngineParameterModulationUnipolar(int trackIndex, int parameter, int source, bool unipolar) override {}
    void setTrackEngineParameterModulationBipolar(int trackIndex, int parameter, int source, bool bipolar) override {}
    void setTrackEngineParameterModulationInvert(int trackIndex, int parameter, int source, bool invert) override {}
    void setTrackEngineParameterModulationScale(int trackIndex, int parameter, int source, float scale) override {}
    void setTrackEngineParameterModulationOffset(int trackIndex, int parameter, int source, float offset) override {}
    void setTrackEngineParameterModulationSmooth(int trackIndex, int parameter, int source, bool smooth) override {}
    void setTrackEngineParameterModulationShape(int trackIndex, int parameter, int source, int shape) override {}
    void setTrackEngineParameterModulationPhase(int trackIndex, int parameter, int source, float phase) override {}
    void setTrackEngineParameterModulationRate(int trackIndex, int parameter, int source, float rate) override {}
    void setTrackEngineParameterModulationDepth(int trackIndex, int parameter, int source, float depth) override {}
    void setTrackEngineParameterModulationSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    
    void setTrackEngineParameterModulationPhaseReset(int trackIndex, int parameter, int source, bool reset) override {}
    void setTrackEngineParameterModulationPhaseResetSync(int trackIndex, int parameter, int source, bool sync) override {}
    void setTrackEngineParameterModulationPhaseResetClockDiv(int trackIndex, int parameter, int source, int clockDiv) override {}
    void setTrackEngineParameterModulationPhaseResetEnabled(int trackIndex, int parameter, int source, bool enabled) override {}
    void setTrack