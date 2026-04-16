#pragma once

#include <concepts>
#include <cstdint>



#define HALF_DT 1.0f / 2.0f	// ms.



// Force the format of the neuron types structures.
template<typename T>
concept NeuronModel = requires(T t)
{
	{ T::a } -> std::convertible_to<float>;
	{ T::b } -> std::convertible_to<float>;
	{ T::c } -> std::convertible_to<float>;
	{ T::d } -> std::convertible_to<float>;
};



// Izhikevich
template<NeuronModel Model>
inline bool step_neuron(float &v, float &u, float I)
{
	// Voltage two-step computing for stability.
	// v(t+dt/2) = v(t) + dt/2 * v'(t)
	auto v_step = [&](float val) {
		return 2 * HALF_DT * (0.04f * val * val + 5.0f * val + 140.0f - u + I);
	};
	v += v_step(v);
	v += v_step(v);

	// Recovery step.
	// u' = a(bv - u)
	u += HALF_DT * Model::a * (Model::b * v - u);

	// Spike detection.
	if (v >= 30.0f)
	{
		v = Model::c;	// Voltage reset.
		u += Model::d;	// After-spike reset.
		return true;
	}
	return false;
}



/**
 * @namespace NeuronModelType
 * @brief All biological neuron type constants.
 */
namespace NeuronModelType
{
	enum Enum : uint8_t
	{
		// Cortex
		CortexPyramidalCell,
		CortexSpinyStellateCell,
		CortexLayer5PyramidalCell,
		CortexChatteringCell,
		CortexBasketCell,
		CortexNonBasketCell,
		CortexMartinottiCell,
		CortexLayer1NonBasketCell,

		// Thalamus
		ThalamusThalamocorticalRelay,
		ThalamusReticularNucleusNeuron,
		ThalamusTIn_FS,
		ThalamusTIn_LTS,

		// Hippocampus
		HippocampusCA3PyramidalCell,
		HippocampusCA1PyramidalCell,
		HippocampusGranuleCell,
		HippocampusBasketCell,
		HippocampusOLMCell,

		// Basal Ganglia
		BasalGangliaMediumSpinyNeuron,
		BasalGangliaStriatalFastSpiking,
		BasalGangliaPallidalNeuron,

		// Brainstem
		BrainstemDopaminergicNeuron,
		BrainstemCholinergicNeuron,
		BrainstemMesencephalicNeuron,

		// Cerebellum
		CerebellumPurkinjeCell,
		CerebellumGranuleCell,
		CerebellumGolgiCell,
		CerebellumStellateCell,

		// Spinal Cord
		SpinalCordAlphaMotorNeuron,
		SpinalCordGammaMotorNeuron,
		SpinalCordRenshawCell,

		// Amygdala
		AmygdalaBasolateralPyramidal,
		AmygdalaCentralNucleusNeuron,

		// Olfactory Bulb
		OlfactoryBulbMitralCell,
		OlfactoryBulbTuftedCell,
		OlfactoryBulbPeriglomerularCell,

		Count // Counter
	};
};



/**
 * @namespace NeuronModels
 * @brief Contains a list of base neuron parameters using models Izhikevich
 */
namespace NeuronModels
{

	// 1. Tonic spiking
	struct TonicSpiking
	{
		static constexpr float a = 0.02f;
		static constexpr float b = 0.2f;
		static constexpr float c = -65.0f;
		static constexpr float d = 6.0f;
	};

	// 2. Phasic spiking
	struct PhasicSpiking
	{
		static constexpr float a = 0.02f;
		static constexpr float b = 0.25f;
		static constexpr float c = -65.0f;
		static constexpr float d = 6.0f;
	};

	// 3. Tonic bursting
	struct TonicBursting
	{
		static constexpr float a = 0.02f;
		static constexpr float b = 0.2f;
		static constexpr float c = -50.0f;
		static constexpr float d = 2.0f;
	};

	// 4. Phasic bursting
	struct PhasicBursting
	{
		static constexpr float a = 0.02f;
		static constexpr float b = 0.25f;
		static constexpr float c = -55.0f;
		static constexpr float d = 0.05f;
	};

	// 5. Mixed mode
	struct MixedMode
	{
		static constexpr float a = 0.02f;
		static constexpr float b = 0.2f;
		static constexpr float c = -55.0f;
		static constexpr float d = 4.0f;
	};

	// 6. Spike frequency adaptation
	struct SpikeFrequencyAdaptation
	{
		static constexpr float a = 0.01f;
		static constexpr float b = 0.2f;
		static constexpr float c = -65.0f;
		static constexpr float d = 8.0f;
	};

	// 7. Class 1 excitable
	struct Class1
	{
		static constexpr float a = 0.02f;
		static constexpr float b = -0.1f;
		static constexpr float c = -55.0f;
		static constexpr float d = 6.0f;
	};

	// 8. Class 2 excitable
	struct Class2
	{
		static constexpr float a = 0.2f;
		static constexpr float b = 0.26f;
		static constexpr float c = -65.0f;
		static constexpr float d = 0.0f;
	};

	// 9. Spike latency
	struct SpikeLatency
	{
		static constexpr float a = 0.02f;
		static constexpr float b = 0.2f;
		static constexpr float c = -65.0f;
		static constexpr float d = 6.0f;
	};

	// 10. Subthreshold oscillations
	struct SubthresholdOscillations
	{
		static constexpr float a = 0.05f;
		static constexpr float b = 0.26f;
		static constexpr float c = -60.0f;
		static constexpr float d = 0.0f;
	};

	// 11. Resonator
	struct Resonator
	{
		static constexpr float a = 0.1f;
		static constexpr float b = 0.26f;
		static constexpr float c = -60.0f;
		static constexpr float d = -1.0f;
	};

	// 12. Integrator
	struct Integrator
	{
		static constexpr float a = 0.02f;
		static constexpr float b = -0.1f;
		static constexpr float c = -55.0f;
		static constexpr float d = 6.0f;
	};

	// 13. Rebound spike (typique des neurones thalamiques)
	struct ReboundSpike
	{
		static constexpr float a = 0.03f;
		static constexpr float b = 0.25f;
		static constexpr float c = -60.0f;
		static constexpr float d = 4.0f;
	};

	// 14. Rebound burst (typique des neurones thalamiques)
	struct ReboundBurst
	{
		static constexpr float a = 0.03f;
		static constexpr float b = 0.25f;
		static constexpr float c = -52.0f;
		static constexpr float d = 0.0f;
	};

	// 15. Threshold variability
	struct ThresholdVariability
	{
		static constexpr float a = 0.03f;
		static constexpr float b = 0.25f;
		static constexpr float c = -60.0f;
		static constexpr float d = 4.0f;
	};

	// 16. Bistability
	struct Bistability
	{
		static constexpr float a = 1.0f;
		static constexpr float b = 1.5f;
		static constexpr float c = -60.0f;
		static constexpr float d = 0.0f;
	};

	// 17. DAP (depolarizing after-potential)
	struct DAP
	{
		static constexpr float a = 1.0f;
		static constexpr float b = 0.2f;
		static constexpr float c = -60.0f;
		static constexpr float d = -21.0f;
	};

	// 18. Accommodation
	struct Accommodation
	{
		static constexpr float a = 0.02f;
		static constexpr float b = 1.0f;
		static constexpr float c = -55.0f;
		static constexpr float d = 4.0f;
	};

	// 19. Inhibition-induced spiking
	struct InhibitionInducedSpiking
	{
		static constexpr float a = -0.02f;
		static constexpr float b = -1.0f;
		static constexpr float c = -60.0f;
		static constexpr float d = 8.0f;
	};

	// 20. Inhibition-induced bursting
	struct InhibitionInducedBursting
	{
		static constexpr float a = -0.026f;
		static constexpr float b = -1.0f;
		static constexpr float c = -45.0f;
		static constexpr float d = 0.0f;
	};

	// 21. Fast spiking
	struct FastSpiking
	{
		static constexpr float a = 0.1f;
		static constexpr float b = 0.2f;
		static constexpr float c = -65.0f;
		static constexpr float d = 2.0f;
	};

};



namespace Cortex
{
	using namespace NeuronModels;


	// EXCITATORY NEURONS

	// Regular Spiking
	using PyramidalCell = SpikeFrequencyAdaptation;
	using SpinyStellateCell = SpikeFrequencyAdaptation;

	// Intrinsically Bursting
	using Layer5PyramidalCell = TonicBursting;

	// Chattering
	using ChatteringCell = MixedMode;


	// INHIBITORY NEURONS 

	// Fast Spiking
	using BasketCell = FastSpiking;

	// Low-Threshold Spiking
	using NonBasketCell = PhasicSpiking;
	using MartinottiCell = PhasicSpiking;

	// Late Spiking
	using Layer1NonBasketCell = SpikeLatency;

}

namespace Thalamus
{
	using namespace NeuronModels;

	// RELAY NEURONS

	// Rebound Burst
	using ThalamocorticalRelay = ReboundBurst;


	// REGULATORY AND INHIBITORY NEURONS

	// Inhibition-Induced Bursting
	using ReticularNucleusNeuron = InhibitionInducedBursting;

	// Fast Spiking
	using TIn_FS = FastSpiking;

	// Low-Threshold Spiking
	using TIn_LTS = PhasicSpiking;

}

namespace Hippocampus
{
	using namespace NeuronModels;

	// EXCITATORY NEURONS

	// Tonic Bursting
	using CA3PyramidalCell = TonicBursting;

	// Regular Spiking
	using CA1PyramidalCell = SpikeFrequencyAdaptation;

	// Tonic Spiking
	using GranuleCell = TonicSpiking;


	// INHIBITORY NEURONS

	// Fast Spiking
	using BasketCell = FastSpiking;

	// Low-Threshold Spiking
	using OLMCell = PhasicSpiking;

}

namespace BasalGanglia
{
	using namespace NeuronModels;

	// STRIATAL NEURONS

	// Spike Latency
	using MediumSpinyNeuron = SpikeLatency;

	// Fast Spiking
	using StriatalFastSpiking = FastSpiking;


	// PALLIDAL NEURONS

	// Tonic Bursting
	using PallidalNeuron = TonicBursting;

}

namespace Brainstem
{
	using namespace NeuronModels;

	// MODULATORY NEURONS

	// Class 1 Excitable
	using DopaminergicNeuron = Class1;

	// Resonator
	using CholinergicNeuron = Resonator;

	// Subthreshold Oscillations
	using MesencephalicNeuron = SubthresholdOscillations;

}

namespace Cerebellum
{
	using namespace NeuronModels;

	// PRINCIPAL NEURONS

	// Depolarizing After-Potential
	using PurkinjeCell = DAP;

	// Tonic Spiking
	using GranuleCell = TonicSpiking;


	// INHIBITORY INTERNEURONS

	// Spike Frequency Adaptation
	using GolgiCell = SpikeFrequencyAdaptation;

	// Phasic Spiking
	using StellateCell = PhasicSpiking;

}

namespace SpinalCord
{
	using namespace NeuronModels;

	// MOTOR NEURONS

	// Integrator
	using AlphaMotorNeuron = Integrator;

	// Class 1 Excitable
	using GammaMotorNeuron = Class1;


	// INHIBITORY INTERNEURONS

	// Tonic Bursting
	using RenshawCell = TonicBursting;

}

namespace Amygdala
{
	using namespace NeuronModels;

	// EXCITATORY NEURONS

	// Regular Spiking
	using BasolateralPyramidal = SpikeFrequencyAdaptation;


	// INHIBITORY NEURONS

	// Spike Latency
	using CentralNucleusNeuron = SpikeLatency;

}

namespace OlfactoryBulb
{
	using namespace NeuronModels;

	// PROJECTION NEURONS

	// Subthreshold Oscillations
	using MitralCell = SubthresholdOscillations;

	// Resonator
	using TuftedCell = Resonator;


	// INHIBITORY INTERNEURONS

	// Phasic Bursting
	using PeriglomerularCell = PhasicBursting;

}
