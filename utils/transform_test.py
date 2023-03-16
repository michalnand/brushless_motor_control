import numpy
import matplotlib.pyplot as plt

def set_torque(torque, phase):
    q = torque*numpy.cos(phase)
    d = torque*numpy.sin(phase)

    return d, q


def set_phases(d, q, theta, control_max = 1):
    #inverse Park transform
    alpha = d*numpy.cos(theta) - q*numpy.sin(theta)
    beta  = d*numpy.sin(theta) + q*numpy.cos(theta)
 
    #inverse Clarke transform
    a = alpha 
    b = -(alpha/2) + beta*numpy.sqrt(3)/2.0
    c = -(alpha/2) - beta*numpy.sqrt(3)/2.0

    #transform into space-vector modulation, to achieve full voltage range
    min_val = min([a, b, c])
    max_val = max([a, b, c])

    com_val = (min_val + max_val)/2.0

    #normalise into 0..control_max
    a_norm = ( (a - com_val)/numpy.sqrt(3) + control_max/2 )
    b_norm = ( (b - com_val)/numpy.sqrt(3) + control_max/2 )
    c_norm = ( (c - com_val)/numpy.sqrt(3) + control_max/2 )

    return alpha, beta, a, b, c, a_norm, b_norm, c_norm

if __name__ == "__main__":

    control_max = 100.0
    phase       = 100*numpy.pi/180.0
    torque      = 100



    steps = 1024

    theta_res   = []
    
    alpha_res      = []
    beta_res      = []

    a_res      = []
    b_res      = []
    c_res      = []

    a_norm_res      = []
    b_norm_res      = []
    c_norm_res      = []

    for n in range(steps):
        theta = (((2*n)%steps)/steps)*2.0*numpy.pi
        
        d, q = set_torque(torque, phase)

        alpha, beta, a, b, c, a_norm, b_norm, c_norm = set_phases(d, q, theta, control_max)

        theta_res.append(theta*180.0/numpy.pi)

        alpha_res.append(alpha)
        beta_res.append(beta)

        a_res.append(a)
        b_res.append(b)
        c_res.append(c)

        a_norm_res.append(a_norm)
        b_norm_res.append(b_norm)
        c_norm_res.append(c_norm)

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


    axs[2].plot(a_res, label="a", color="red")
    axs[2].plot(b_res, label="b", color="green")
    axs[2].plot(c_res, label="c", color="blue")
    axs[2].set_xlabel("step")
    axs[2].set_ylabel("current")
    axs[2].legend()
    axs[2].grid()


    axs[3].plot(a_norm_res, label="PWM a", color="red")
    axs[3].plot(b_norm_res, label="PWM b", color="green")
    axs[3].plot(c_norm_res, label="PWM c", color="blue")
    axs[3].set_xlabel("step")
    axs[3].set_ylabel("pwm normalised")
    axs[3].legend()
    axs[3].grid()
        
    plt.tight_layout()
    #plt.show()
    plt.savefig("output.png", dpi = 300)