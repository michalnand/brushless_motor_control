import numpy
import matplotlib.pyplot as plt


def set_phases(d, q, theta, pwm_max):
    #inverse Park transform
    alpha = d*numpy.cos(theta) - q*numpy.sin(theta)
    beta  = d*numpy.sin(theta) + q*numpy.cos(theta)

    #inverse Clarke transform
    ia = alpha 
    ib = -(alpha/2) + beta*numpy.sqrt(3)/2.0
    ic = -(alpha/2) - beta*numpy.sqrt(3)/2.0

    #transform into space-vector modulation, to achieve full voltage range
    min_val = min([ia, ib, ic])
    max_val = max([ia, ib, ic])

    com_val = (min_val + max_val)/2.0

    #normalise into 0..pwm_max
    ia_norm = ( (ia - com_val)/numpy.sqrt(3) + pwm_max/2 )
    ib_norm = ( (ib - com_val)/numpy.sqrt(3) + pwm_max/2 )
    ic_norm = ( (ic - com_val)/numpy.sqrt(3) + pwm_max/2 )

    return alpha, beta, ia, ib, ic, ia_norm, ib_norm, ic_norm

if __name__ == "__main__":

    pwm_max = 100.0
    amp     = pwm_max*1.0
    phase   = 0*numpy.pi/180.0
    d = amp*numpy.cos(phase)
    q = amp*numpy.sin(phase)

    steps = 1024

    theta_res   = []
    
    alpha_res      = []
    beta_res      = []

    ia_res      = []
    ib_res      = []
    ic_res      = []

    ia_norm_res      = []
    ib_norm_res      = []
    ic_norm_res      = []

    for n in range(steps):
        theta = (((2*n)%steps)/steps)*2.0*numpy.pi
        

        alpha, beta, ia, ib, ic, ia_norm, ib_norm, ic_norm = set_phases(d, q, theta, pwm_max)

        theta_res.append(theta*180.0/numpy.pi)

        alpha_res.append(alpha)
        beta_res.append(beta)

        ia_res.append(ia)
        ib_res.append(ib)
        ic_res.append(ic)

        ia_norm_res.append(ia_norm)
        ib_norm_res.append(ib_norm)
        ic_norm_res.append(ic_norm)

    plt.clf()
    fig, axs = plt.subplots(4, 1, figsize=(8, 8))


    axs[0].plot(theta_res, label="theta", color="deepskyblue")
    axs[0].set_xlabel("rotor step")
    axs[0].set_ylabel("angle [degrees]")
    axs[0].grid()

    axs[1].plot(alpha_res, label="alpha", color="red")
    axs[1].plot(beta_res, label="beta", color="blue")
    axs[1].set_xlabel("step")
    axs[1].set_ylabel("phase")
    axs[1].legend()
    axs[1].grid()


    axs[2].plot(ia_res, label="Ia", color="red")
    axs[2].plot(ib_res, label="Ib", color="green")
    axs[2].plot(ic_res, label="Ic", color="blue")
    axs[2].set_xlabel("step")
    axs[2].set_ylabel("current")
    axs[2].legend()
    axs[2].grid()


    axs[3].plot(ia_norm_res, label="PWM a", color="red")
    axs[3].plot(ib_norm_res, label="PWM b", color="green")
    axs[3].plot(ic_norm_res, label="PWM c", color="blue")
    axs[3].set_xlabel("step")
    axs[3].set_ylabel("pwm normalised")
    axs[3].legend()
    axs[3].grid()
        
    plt.tight_layout()
    #plt.show()
    plt.savefig("output.png", dpi = 300)