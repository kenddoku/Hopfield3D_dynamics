#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <cmath>
#include <vector>
#include <tuple>
#include <string>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_sf_gamma.h>

namespace fs = std::filesystem;

/*
 * ##################################################################################################
 *      [w11 w12 w13]                                                                               |
 *  W = [w21 w22 w23] , where wij is the weight between i-th and j-th neurons                       |
 *      [w31 w32 w33]                                                                               |
 *                                                                                                  |
 *  The following convention was adopted: (1 -> x), (2 -> y), (3 -> z) as this model operates with  |
 *  only three neurons.                                                                             |
 *                                                                                                  |
 * ##################################################################################################
 */ 
struct Params
{
    double nu;

    double x0, y0, z0;

    double w11, w12, w13;
    double w21, w22, w23;
    double w31, w32, w33;

    int n_iter;

    Params(std::string filename_wparams_) {setParams(filename_wparams_);}

private:
    void setParams(const std::string& filename)
    {
        std::ifstream file(filename);
        
        if(!file.is_open())
        {
            std::cerr << "ERROR opening " << filename << std::endl;
            return;
        }

        std::string line;

        // getting nu parameter
        std::getline(file, line);
        nu = std::stod(line);

        // getting initial state of the system: x0, y0, z0
        std::getline(file, line);
        std::istringstream iss_xyz0(line);

        std::vector<double> XYZ0;
        double xyz0;
        while(iss_xyz0 >> xyz0)
        {
            XYZ0.push_back(xyz0);
        }

        x0 = XYZ0[0]; y0 = XYZ0[1]; z0 = XYZ0[2];

        // getting the weights w11, w12, ..., w32, w33
        std::vector<double> W;
        while(std::getline(file, line))
        {
            std::istringstream iss_w(line);

            double wvalue;
            while(iss_w >> wvalue)
            {
                W.push_back(wvalue);
            }
        }

        w11 = W[0]; w12 = W[1]; w13 = W[2];
        w21 = W[3]; w22 = W[4]; w23 = W[5];
        w31 = W[6]; w32 = W[7]; w33 = W[8];

        std::getline(file, line);
        n_iter = std::stoi(line);

        return;
    }
};

class HopfieldNetwork
{
    double x0, y0, z0;
    Params* wp; // wp - weight parameters including nu which is the order of fractional difference equation (wparams)
    int n_iter;

public:
    std::vector<double> x, y, z;

    // Constructor enabling user to specify initial state of the system
    HopfieldNetwork(double x0_, double y0_, double z0_, void* wparams_, int n_iter_=-1) : x0(x0_), y0(y0_), z0(z0_)
    {
        wp = static_cast<Params*>(wparams_);

        // Number of time-evol iterations
        if(n_iter_ == -1) n_iter = wp->n_iter;
        else n_iter = n_iter_;

        // Vectors holding the neurons' state
        x = std::vector<double>(n_iter, 0.0);
        y = std::vector<double>(n_iter, 0.0);
        z = std::vector<double>(n_iter, 0.0);

        x[0] = x0;
        y[0] = y0;
        z[0] = z0;
    }

    // Constructor that uses the initial state of the system specified in the file
    HopfieldNetwork(void* wparams_)
    {
        wp = static_cast<Params*>(wparams_);

        n_iter = wp->n_iter;

        x0 = wp->x0;
        y0 = wp->y0;
        z0 = wp->z0;

        // Vectors holding the neurons' state
        x = std::vector<double>(n_iter, 0.0);
        y = std::vector<double>(n_iter, 0.0);
        z = std::vector<double>(n_iter, 0.0);

        x[0] = x0;
        y[0] = y0;
        z[0] = z0;
    }

    void displayParams()
    {
        std::cout << wp->w11 << " " << wp->w12 << " " << wp->w13 << '\n';
        std::cout << wp->w21 << " " << wp->w22 << " " << wp->w23 << '\n';
        std::cout << wp->w31 << " " << wp->w32 << " " << wp->w33 << '\n';
        std::cout << wp->nu << '\n';
        std::cout << wp->n_iter << '\n';
    }

    // Method that computes the states of all three neurons in n_iter steps, optionally saves all these states to file (saveToFile=true)
    // and returns a tuple holding three 1D vectors, one for each neuron x, y and z, that hold full information about system's evolution
    void solve(const std::string& filename="")
    {
        // Creating file for results and writing initial state of the system (n=0)
        std::ofstream file(filename);
        if(!file)
        {
            std::cerr << "ERROR opening " << filename << '\n';
            return;
        }
        file << "n,x,y,z\n";
        file << 0 << "," << std::fixed << std::setprecision(9) << x[0] << "," << y[0] << "," << z[0] << '\n';

        // calculating value of gamma function for given 'nu'
        double gammanu = gsl_sf_gamma(wp->nu);

        // Creating variables/objects used for caching repetetive values to avoid ------------------------------------------------------
        // unnecessary computations
        
        /* Since computing gamma functions of large values leads to numeric overflow I will use a trick:
        * Instead of calculating gamma(a)/gamma(b), where a=n-j+nu, b=n-j+1, one can calculate a natural logarithm of this
        * fraction using gsl_sf_lngamma: alpha = ln( gamma(a) / gamma(b)) = ln( gamma(a) ) - ln( gamma(b) )
        * Once the alpha is calculated (which is not supposed to be an enormous number) we can simply exponentiate it 
        * and get the final result used for further calculations:
        *
        * gammafrac = std::exp(alpha) */
        std::vector<double> gammafrac_cache(n_iter, 0.0);
        for(int j=1; j<n_iter; j++) {
            const int n_max = n_iter - 1; // must substract one cuz 'n' is always less than 'n_iter' (look at nested for loops below)
            
            double alpha {0.0};
            alpha = gsl_sf_lngamma(n_max-j+wp->nu) - gsl_sf_lngamma(n_max-j+1);
            gammafrac_cache[n_max-j] = std::exp(alpha);
        }
        std::cout << "gammafrac_cache vector created...\n";

        /* Vectors initialized right below are used to store results of repetitive calculations of this kind:
            for(int n=1; n<n_iter; n++)    
                for(int j=1; j<n; j++) {
                    xnsum += gammafrac * (
                        -x[j-1] +                       |
                        wp->w11*std::tanh(x[j-1]) +     |~~~> this is what's getting stored in xjsum_cache[j-1]
                        wp->w12*std::tanh(y[j-1]) +     |
                        wp->w13*std::tanh(z[j-1])       |
                    );
                }
        */
        std::vector<double> xjsum_cache(n_iter, 0.0);
        std::vector<double> yjsum_cache(n_iter, 0.0);
        std::vector<double> zjsum_cache(n_iter, 0.0);

        xjsum_cache[0] = -x[0] +
                    wp->w11*std::tanh(x[0]) +
                    wp->w12*std::tanh(y[0]) +
                    wp->w13*std::tanh(z[0]);

        yjsum_cache[0] = -y[0] +
                    wp->w21*std::tanh(x[0]) +
                    wp->w22*std::tanh(y[0]) +
                    wp->w23*std::tanh(z[0]);

        zjsum_cache[0] = -z[0] +
                    wp->w31*std::tanh(x[0]) +
                    wp->w32*std::tanh(y[0]) +
                    wp->w33*std::tanh(z[0]);

        for(int n=1; n<n_iter; n++)
        {
            double xnsum {0};
            double ynsum {0};
            double znsum {0};

            for(int j=1; j<=n; j++)
            {   
                xnsum += gammafrac_cache[n-j] * xjsum_cache[j-1];

                ynsum += gammafrac_cache[n-j] * yjsum_cache[j-1];

                znsum += gammafrac_cache[n-j] * zjsum_cache[j-1];
            }

            // Caclulating the next step and writing it to the file
            x[n] = x[0] + xnsum / gammanu;
            y[n] = y[0] + ynsum / gammanu;
            z[n] = z[0] + znsum / gammanu;
            file << n << "," << std::fixed << std::setprecision(9) << x[n] << "," << y[n] << "," << z[n] << '\n';

            xjsum_cache[n] = -x[n] +
                    wp->w11*std::tanh(x[n]) +
                    wp->w12*std::tanh(y[n]) +
                    wp->w13*std::tanh(z[n]);

            yjsum_cache[n] = -y[n] +
                    wp->w21*std::tanh(x[n]) +
                    wp->w22*std::tanh(y[n]) +
                    wp->w23*std::tanh(z[n]);

            zjsum_cache[n] = -z[n] +
                    wp->w31*std::tanh(x[n]) +
                    wp->w32*std::tanh(y[n]) +
                    wp->w33*std::tanh(z[n]);
        }
        file.close();
        
        return;
    }

/*
    void bifurcation(std::ofstream& filex, std::ofstream& filey, std::ofstream& filez)
    {
        int nstepsLast {100}; // number of steps to save (counted from the end of neuron's evolution vector)
        std::vector<double> xb(nstepsLast, 0), yb(nstepsLast, 0), zb(nstepsLast, 0); // vectors holding last nstepsLast steps of neurons' evolution

        auto [x, y, z] = solve(n_iter);

        for(int i=0; i<nstepsLast; ++i)
        {
            int indx = n_iter-1-i;

            xb[i] = x[indx];
            yb[i] = y[indx];
            zb[i] = z[indx];

            filex << "," << xb[i];
            filey << "," << yb[i];
            filez << "," << zb[i];
        }
        filex << '\n';
        filey << '\n';
        filez << '\n';
    }
*/
    // THIS FEATURE IS NOT USED CURRENTLY!!!!
    // Overloading operator: H1->H2 means that H1 (transmitter) sends synchronisation signal to H2 (receiver) which results in
    // synchronisation of these two Hopfield networks
    // void operator*(HopfieldNetwork& other) // here other is the receiver that is to be synchronised with the transmitter
    // {
    //     int nstepsChaos {50}; // number of steps to take in two networks befor synchronisation begins
    //     int nstepsSynchro {2000}; // number of steps that will be used for synchronisation

    //     auto [xtemp, ytemp, ztemp] = solve(nstepsChaos);
    //     auto [xslavetemp, yslavetemp, zslavetemp] = other.solve(nstepsChaos);

    //     std::ofstream fileTransmitter("transmitter.csv");
    //     std::ofstream fileReceiver("receiver.csv");
    //     std::ofstream fileSynchroError("synchroError.csv");

    //     fileTransmitter << "n,x,y,z\n";
    //     fileReceiver << "n,x,y,z\n";
    //     fileSynchroError << "n,ex,ey,ez\n";

    //     x[0] = x[nstepsChaos-1];
    //     y[0] = y[nstepsChaos-1];
    //     z[0] = z[nstepsChaos-1];

    //     double gammanu = gsl_sf_gamma(wp->nu);

    //     for(int n=1; n<nstepsSynchro; n++)
    //     {
    //         double xnsum {0};
    //         double ynsum {0};
    //         double znsum {0};

    //         // other.x, other.y and other.z are equivalent to xslave, yslave, zslave
    //         double xslavensum {0};
    //         double yslavensum {0};
    //         double zslavensum {0};

    //         // Synchronisation errors
    //         double ex {};
    //         double ey {};
    //         double ez {};

    //         double k_safety = 1; // k_safety is to ensure that -1 < max{k_i} < 2^{nu} - 1

    //         // k1 = kx, k2 = ky, k3 = kz
    //         double k1 = 0.6311; // {std::pow(2, wp->nu) - 1 - k_safety}; 
    //         double k2 = 0.6311; // {std::pow(2, wp->nu) - 1 - k_safety}; 
    //         double k3 = 0.6311; // {std::pow(2, wp->nu) - 1 - k_safety}; 

    //         for(int j=1; j<=n; j++)
    //         {
    //             /* Computing gammafrac in the same way as in solve() method */
    //             double alpha = gsl_sf_lngamma(n-j+wp->nu) - gsl_sf_lngamma(n-j+1);
    //             double gammafrac = std::exp(alpha);

    //             // Computing x and xslave state - synchronising -----------------------------------
    //             xnsum += gammafrac * (
    //                 -x[j-1] +
    //                 wp->w11*std::tanh(x[j-1]) +
    //                 wp->w12*std::tanh(y[j-1]) +
    //                 wp->w13*std::tanh(z[j-1])
    //             );

    //             // IF x state is NOT being sent
    //             // xslavensum += gammafrac * (
    //             //     -other.x[j-1] +
    //             //     wp->w11*std::tanh(other.x[j-1]) +
    //             //     wp->w12*std::tanh(other.y[j-1]) +
    //             //     wp->w13*std::tanh(other.z[j-1])
    //             // );

    //             // IF x state is being sent:
    //             xslavensum += gammafrac * (
    //                 -other.x[j-1] +
    //                 wp->w11*std::tanh(other.x[j-1]) +
    //                 wp->w12*std::tanh(other.y[j-1]) +
    //                 wp->w13*std::tanh(other.z[j-1]) -
    //                 wp->w11*( std::tanh(other.x[j-1]) - std::tanh(x[j-1]) ) -
    //                 wp->w12*( std::tanh(other.y[j-1]) - std::tanh(y[j-1]) ) -
    //                 wp->w13*( std::tanh(other.z[j-1]) - std::tanh(z[j-1]) ) -
    //                 k1*( other.x[j-1] - x[j-1] )
    //             );

    //             // Computing y and yslave state - synchronising -----------------------------------
    //             ynsum += gammafrac * (
    //                 -y[j-1] +
    //                 wp->w21*std::tanh(x[j-1]) +
    //                 wp->w22*std::tanh(y[j-1]) +
    //                 wp->w23*std::tanh(z[j-1])
    //             );

    //             // IF y state is NOT being sent:
    //             yslavensum += gammafrac * (
    //                 -other.y[j-1] +
    //                 wp->w21*std::tanh(other.x[j-1]) +
    //                 wp->w22*std::tanh(other.y[j-1]) +
    //                 wp->w23*std::tanh(other.z[j-1])
    //             );

    //             // IF y state is being sent:
    //             // yslavensum += gammafrac * (
    //             //     -other.y[j-1] +
    //             //     wp->w21*std::tanh(other.x[j-1]) +
    //             //     wp->w22*std::tanh(other.y[j-1]) +
    //             //     wp->w23*std::tanh(other.z[j-1]) -
    //             //     wp->w21*( std::tanh(other.x[j-1]) - std::tanh(x[j-1]) ) -
    //             //     wp->w22*( std::tanh(other.y[j-1]) - std::tanh(y[j-1]) ) -
    //             //     wp->w23*( std::tanh(other.z[j-1]) - std::tanh(z[j-1]) ) -
    //             //     k2*( other.y[j-1] - y[j-1] )
    //             // );

    //             // Computing z and zslave state - synchronising -----------------------------------
    //             znsum += gammafrac * (
    //                 -z[j-1] +
    //                 wp->w31*std::tanh(x[j-1]) +
    //                 wp->w32*std::tanh(y[j-1]) +
    //                 wp->w33*std::tanh(z[j-1])
    //             );

    //             // IF z state is NOT being sent:
    //             zslavensum += gammafrac * (
    //                 -other.z[j-1] +
    //                 wp->w31*std::tanh(other.x[j-1]) +
    //                 wp->w32*std::tanh(other.y[j-1]) +
    //                 wp->w33*std::tanh(other.z[j-1])
    //             );

    //             // IF z state is being sent:
    //             // zslavensum += gammafrac * (
    //             //     -other.z[j-1] +
    //             //     wp->w31*std::tanh(other.x[j-1]) +
    //             //     wp->w32*std::tanh(other.y[j-1]) +
    //             //     wp->w33*std::tanh(other.z[j-1]) -
    //             //     wp->w31*( std::tanh(other.x[j-1]) - std::tanh(x[j-1]) ) -
    //             //     wp->w32*( std::tanh(other.y[j-1]) - std::tanh(y[j-1]) ) -
    //             //     wp->w33*( std::tanh(other.z[j-1]) - std::tanh(z[j-1]) ) -
    //             //     k3*( other.z[j-1] - z[j-1] )
    //             // );
    //         }

    //         x[n] = x[0] + xnsum / gammanu;
    //         y[n] = y[0] + ynsum / gammanu;
    //         z[n] = z[0] + znsum / gammanu;

    //         other.x[n] = other.x[0] + xslavensum / gammanu;
    //         other.y[n] = other.y[0] + yslavensum / gammanu;
    //         other.z[n] = other.z[0] + zslavensum / gammanu;

    //         ex = other.x[n] - x[n];
    //         ey = other.y[n] - y[n];
    //         ez = other.z[n] - z[n];

    //         fileTransmitter << n << ',' << x[n] << ',' << y[n] << ',' << z[n] << '\n';
    //         fileReceiver << n << ',' << other.x[n] << ',' << other.y[n] << ',' << other.z[n] << '\n';
    //         fileSynchroError << n << ',' << ex << ',' << ey << ',' << ez << '\n';
    //     }

    //     fileTransmitter.close();
    //     fileReceiver.close();
    //     fileSynchroError.close();
    // }

};

int main(int argc, char* argv[])
{
    fs::path paramsPath = argv[1];
    fs::path resultPath = argv[2]; 

    Params wparams(paramsPath);

    HopfieldNetwork H(&wparams);
    H.solve(resultPath);

    return 0;
}
