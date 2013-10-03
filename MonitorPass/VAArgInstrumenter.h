/**
 * @file VAArgInstrumenter.h
 * @brief VAArgInstrumenter Declarations.
 */

#ifndef VA_ARG_INSTRUMENTER_H_
#define VA_ARG_INSTRUMENTER_H_

class VAArgInstrumenter : public UninterpretedInstrumenter {
  public:
    VAArgInstrumenter(std::string name, Instrumentation* instrumentation) :
      UninterpretedInstrumenter(name, instrumentation) {};
};

#endif /* VA_ARG_INSTRUMENTER_H_ */
