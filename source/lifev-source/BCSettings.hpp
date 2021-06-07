#include <iostream>
#include <string>

#define PI 3.14159265

typedef std::function<Real ( Real const&, Real const&, Real const&, Real const&, ID const& ) > fct_Type;

class BCSettings
{
public:

    /** @name Constructors, destructor
     */
    BCSettings (Real p, Real s, Real f);

    ~BCSettings ()
    {}

    /** @name  Methods0
     */

    void set_P (Real p)
    {
        P = p;
    }

    void set_stroke (Real s)
    {
        STROKE = s;
    }

    void set_frequency (Real f)
    {
        FREQUENCY = f;
    }

    Real get_P ()
    {
        return P;
    }

    Real get_stroke ()
    {
        return STROKE;
    }    

    Real get_frequency ()
    {
        return FREQUENCY;
    }

    Real interpolate (Real data_a, Real data_b, Real t_a, Real t_b, Real t_star)
    {
    	return ( data_a + ((t_star-t_a)/(t_b-t_a))*data_b );
    }  


    // Homogenous condition
    Real zeroFunction (const Real& /*t*/, const Real& /*x*/, const Real& /*y*/, const Real& /*z*/, const ID& /*i*/);
    fct_Type get_zeroFunction();

    // Neumann BC function: constant value on the inlet ring surface
    Real pressureFunction (const Real& /*t*/, const Real& /*x*/, const Real& /*y*/, const Real& /*z*/, const ID& i);
    fct_Type get_pressureFunction();

    /// BC for the disc oscillation, imposed by the titanium holder
    Real strokeFunction (const Real& t, const Real& /*x*/, const Real& /*y*/, const Real& /*z*/, const ID& i);
    fct_Type get_strokeFunction();

    Real strokeDerivativeFunction(const Real& t, const ID& i);
  

private:

    Real P;  	      /**< outlet pressure of the pump (in dyne/cm^2) */
    Real STROKE; 	  /**< amplitude of the membrane oscillation (in cm) */
    Real FREQUENCY;   /**< frequency of the membrane oscillation (in Hz) */

};

BCSettings::BCSettings ( Real p, Real s, Real f)
{
    P = p;
    STROKE = s;
    FREQUENCY = f;
}

// Homogenous condition
Real BCSettings::zeroFunction (const Real& /*t*/, const Real& /*x*/, const Real& /*y*/, const Real& /*z*/, const ID& /*i*/)
{
    return 0.;
}
fct_Type BCSettings::get_zeroFunction()
{
    fct_Type f;
    f = std::bind (&BCSettings::zeroFunction, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    return f;
}

// Neumann BC function: constant value on the inlet ring surface
Real BCSettings::pressureFunction (const Real& /*t*/, const Real& /*x*/, const Real& /*y*/, const Real& /*z*/, const ID& i)
{
    if (i==2)
        return P*1333.22;
    return 0.0;
}
fct_Type BCSettings::get_pressureFunction()
{
    fct_Type f;
    f = std::bind (&BCSettings::pressureFunction, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    return f;
}

/// BC for the disc oscillation, imposed by the titanium holder
Real BCSettings::strokeFunction (const Real& t, const Real& /*x*/, const Real& /*y*/, const Real& /*z*/, const ID& i)
{
    if (i == 2)
    {
        return STROKE*sin(2*PI*FREQUENCY*t);
    }
    return 0.0;
}
fct_Type BCSettings::get_strokeFunction()
{
    fct_Type f;
    f = std::bind (&BCSettings::strokeFunction, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    return f;
}

/// BC for the disc oscillation, imposed by the titanium holder
Real BCSettings::strokeDerivativeFunction (const Real& t, const ID& i)
{
    if (i == 2)
    {
        return 2*PI*FREQUENCY*STROKE*cos(2*PI*FREQUENCY*t);
    }
    return 0.0;
}