#ifndef AMPGEN_FASTCOHERENTSUM_H
#define AMPGEN_FASTCOHERENTSUM_H

#include <memory.h>
#include <stddef.h>
#include <complex>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "AmpGen/AmplitudeRules.h"
#include "AmpGen/CompiledExpression.h"
#include "AmpGen/EventList.h"
#include "AmpGen/EventType.h"
#include "AmpGen/Integrator.h"
#include "AmpGen/Types.h"
#include "AmpGen/Event.h"

namespace AmpGen
{
  class LinearErrorPropagator;
  class MinuitParameter;
  class MinuitParameterSet;
  class FitFraction;
  class Particle;

  class CoherentSum
  {
  public:
    CoherentSum( const EventType& type, const AmpGen::MinuitParameterSet& mps, const std::string& prefix = "" );
    virtual ~CoherentSum() = default; 

    AmplitudeRules protoAmplitudes() { return m_protoAmplitudes; }
    std::string prefix() const { return m_prefix; }
    
    TransitionMatrix<complex_t> operator[]( const size_t& index ) { return m_matrixElements[index]; }
    const TransitionMatrix<complex_t> operator[]( const size_t& index ) const { return m_matrixElements[index]; }
    size_t size() const { return m_matrixElements.size(); }
    
    real_t getWeight() const { return m_weight; }
    real_t operator()( const Event& evt ) const { return prob( evt ); }
    real_t prob( const Event& evt ) const { return m_weight * std::norm( getVal( evt ) ) / m_norm; }
    real_t prob_unnormalised( const Event& evt ) const { return std::norm( getVal( evt ) ); }
    real_t norm( const Bilinears& norms ) const;
    real_t norm() const;
    real_t getNorm( const Bilinears& normalisations );
    real_t get_norm() const { return m_norm ; }

    complex_t norm( const unsigned int& x, const unsigned int& y ) const;
    complex_t getVal( const Event& evt ) const;
    complex_t getVal( const Event& evt, const std::vector<unsigned int>& cacheAddresses ) const;
    complex_t getValNoCache( const Event& evt ) const;
    
    void transferParameters();
    void prepare();
    void printVal( const Event& evt );
    void updateNorms( const std::vector<unsigned int>& changedPdfIndices );
    void setWeight( const double& weight ) { m_weight = weight; }
    void setWeight( MinuitParameter* param ) { m_weightParam = param; }
    void makeTotalExpression();
    void reset( bool resetEvents = false );
    void setEvents( EventList& list );
    void setMC( EventList& sim );
    void PConjugate();
    void debug( const Event& evt, const std::string& nameMustContain="");
    void generateSourceCode( const std::string& fname, const double& normalisation = 1, bool add_mt = false );
    void resync();

    std::vector<unsigned int> cacheAddresses( const EventList& evts ) const; 
    std::vector<FitFraction> fitFractions( const LinearErrorPropagator& linProp );
    std::vector<TransitionMatrix<complex_t>> matrixElements() const { return m_matrixElements; }

    std::map<std::string, std::vector<unsigned int>> getGroupedAmplitudes();
    Bilinears norms() const { return m_normalisations ; }
  
  protected:
    std::vector<TransitionMatrix<complex_t>> m_matrixElements; ///< Vector of (expanded) matrix elements
    Bilinears m_normalisations;                  ///< Normalisation integrals
    AmplitudeRules m_protoAmplitudes;            ///< Proto amplitudes from user rule-set
    Integrator<10> m_integrator;                 ///< Integral dispatch tool (with default unroll = 10) 
    TransitionMatrix<complex_t> m_total;         ///< Total Matrix Element 
    EventList* m_events            = {nullptr};  ///< Data events to evaluate PDF on
    EventType  m_evtType;                        ///< Final state for this amplitude
    MinuitParameter* m_weightParam = {nullptr};  ///< Weight parameter (i.e. the normalised yield)
    size_t m_prepareCalls          = {0};        ///< Number of times prepare has been called
    size_t m_lastPrint             = {0};        ///< Last time verbose PDF info was printed
    size_t m_printFreq             = {0};        ///< Frequency to print verbose PDF info
    double m_weight                = {1};        ///< Weight number (i.e. the normalised yield)
    double m_norm                  = {0};        ///< Normalisation integral
    std::string m_prefix           = {""};       ///< Prefix for matrix elements
    bool m_isConstant              = {false};    ///< Flag for a constant PDF
    bool m_dbThis                  = {false};    ///< Flag to generate amplitude level debugging
    bool m_verbosity               = {false};    ///< Flag for verbose printing

    void addMatrixElement( std::pair<Particle, CouplingConstant>& particleWithCoupling, const MinuitParameterSet& mps );
    bool isFixedPDF(const MinuitParameterSet& mps) const;

  };
} // namespace AmpGen

#endif
