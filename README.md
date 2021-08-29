# 5G SCG: Cardiovascular Activity Monitoring Using mmWaves

Motivated by recent advances in human vital sign technology, I want to leverage the power of RF-sensing to monitor cardiac activity. This project investigates the potential of leveraging mmWaves, a specific band of radio frequencies used in emerging 5G networks, to monitor cardiovascular health activity. I have built an extendable cardiovascular monitoring platform using off-the-shelf mmWave hardware and used it to capture cardiovascular recordings that resemble a Seismocardiogram. My empirical investigation demonstrates that the non-contact measurements collected using my platform are are comparable to those collected with on-body sensors. Looking ahead, this system could be used to diagnose and monitor a variety of cardiovascular illnesses and conditions.

## System Summary

At a high level, my contactless RF-based vital sign monitoring system works as follows: an RF signal is transmitted toward a persons’ chest and then the reflection is captured by a receiving antenna. The distance traveled can be calculated by analyzing the phase shift in the signal which changes linearly with distance (i.e. $\phi=2\pi~\frac{d}{\lambda}$). By tracking small changes in the distance we can measure cardiac vibrations in the chest. These vibrations correspond to myocardial movements caused by the contraction and expansion of the heart muscles. Specifically, the physiological phenomenon that allows wireless signals to extract heart rate using signal reflections from the chest is called seismocardiography (SCG). Past work has shown that an SCG signal contains fiducial points which map to a physiologic event such as atrial inflow and heart valves opening or closing. These fiducial points have been recorded using an on-body accelerometer but have never been measured using radio waves. 

## System Design and Implementation

### Hardware Components
- IWR1443BOOST mmWave Sensor Module
- DCA1000EVM Real-time Data-capture Adapter
- USRP N210

### Software Components
- C for Real-time Data Processing
- Boost Multi-threading Library
- MATLAB for Plotting and Signal Processing

## Why mmWaves?

The system utilizes a specific band of radio frequencies called mmWaves. Due to their high frequency, we can detect very small movements that
other frequency bands simply cannot. This makes it possible to record a high resolution SCG signal which contains the necessary information for detecting the
opening and closing of heart valves. In addition to their high frequency, mmWaves are also one of the technologies behind 5G networks. As 5G networks become ubiquitous in the coming years mmWaves chips will be integrated into all next-generation devices. Newer smartphones like the Pixel 4 already contain mmWave sensors that can detect hand gestures.

As mmWave sensors become more common in smartphones they will open up a whole host of new possibilities to users at no extra cost to them. My research could one day make it possible for lay users to monitor their cardiovascular activity using just their smartphones. This would increase accessibility to healthcare by creating a low-cost method of monitoring your cardiovascular activity. Moreover, once 5G routers are integrated into homes and office they can be used to passively monitor the physical health of the occupants. As health-monitoring technologies advance further, I envision a future where your smartphone can be used to accurately monitor cardiovascular conditions.


## Summary of Seismocardiography

Seismocardiogram (SCG) represents the local vibrations of the chest wall in response to the heartbeat. Seismocardiography was first studied in the 1960s by R. Baevski who was inspired by the technology used in seismology to register underground vibration and predict earthquakes. Baevski adapted the technology to measure fluctuations caused by precordial movements.

SCG has been used in aerospace programs to monitor crew health. The first was used by Russian cosmonauts in 1963 onboard the Vostok 5, then again in the early seventies onboard the spacecraft "Sojus" and even onboard the International Space Station since March 2007 as part of the Pneumocard and Sonocard experiments.

SCG was not introduced into clinical medicine until 1990 by Salerno and Zanetti to monitoring changes in left ventricular function during ischemia. The SCG waveform was given a nomenclature by R. Crow in 1994 when he recorded seismocardiography simultaneously with echocardiography and analyzed the relationship of the event in the cardiac cycle.

Two large clinical trials demonstrated SCG was more sensitive and specific than 15 ECG in detecting coronary artery disease during ECG stress exercise testing. It was also shown that when the ECG and SCG results were combined, the sensitivity and specificity of the combined test was comparable to radionuclide and ECHO imaging techniques.

SCG was largely abandoned by the medical community due to the overly-cumbersome apparatus needed to measure the on-body accelerations and the advent of echocardiography and magnetic resonance imaging. However, recent developments have produced a plethora of high quality, sensitive, inexpensive accelerometers ideal for recording and analyzing the heart’s vibrational output.

The primary advantage of wearable SCG measurement systems is the possibility of obtaining data continuously. In 2007, a team proposed an integrated vest with an accelerometer inside and placed in contact with the subject’s sternum. They recorded 24 hours of data while the subject performed a variety of activities.

SCG was originally proposed as a diagnostic tool for clinics. A subject would have their SCG activity recorded and a physician would read the recordings to make a diagnosis. This was later abandoned due to the large intersubject variability of SCG recordings. However, studies found that the intrasubject variability in the signals over serial measurements was low, except when the subject was experiencing a change in cardiovascular health. For this reason, SCG became a tool for monitoring changes in the same patient’s health over time, because the subject would be their own control, making intersubject variability no longer an obstacle.

## System Evaluation

### Fiducial Point Detection

While existing literature has show that the SCG signal has been used to detect many cardiological events we chose to focus on the opening and closing of the aortic and mitral valves.  These four points were consistently found in past literature and seem to be the most visible in the regular seismocardiogram.

### mmWave vs. Accelerometer

When comparing a segment of the SCG signal recorded using the mmWave sensor to the SCG signal recorded using an on-body accelerometer we see that both signals have a similar morphology. In the systolic, we see a peak that corresponds to the aortic valve opening (AO) and in the diastolic, there is a trough that corresponds to the mitral valve opening (MO).

### Averaged SCG Results

First, the SCG data collected with the mmWave sensor was segmented using the R-peaks of the ECG signal. Second, the signals were then layered with the same starting point. Third, the mean length of the SCG segments was calculated and all the segments were time-scaled to that length. Fourth, the signals were averaged together linearly to produce a mean ECG signal. Once we had the mean SCG signal, I could use the information from existing literature to identify fiducial points. Specifically, I began by identifying the Atrial Valve Opening (AO) in the systolic and the Mitral Valve Opening (MO) in the diastolic since they were the most prominent points. It is then easy to infer the location of the Mitral Valve Closing (MC) because it will be the peak immediately preceding the AO and the Atrial Valve Closing (AC) will be the peak immediately preceding the MO. In some trials, it was also possible to identify the Peak Atrial Systole (AS), a large peak proceeding the systole portion, but this point was not consistently found so it was omitted from the results. The inconsistent morphology of SCG signals is a common issue which I will discuss further in the limitations section.

## Application for Seismocardiography

### Cardiovascular Activity Monitoring

#### Pulse Transit Time

#### Pulse Wave Velocity

#### Blood Pressure Monitoring

### Cardiovascular Disease Detection

#### Atrial Fibrillation

#### Atrial Flutter

## Publications

Ha, Unsoo, Salah Assana, and Fadel Adib. "Contactless seismocardiography via deep learning radars." _Proceedings of the 26th Annual International Conference on Mobile Computing and Networking_. 2020. [\[PDF\]](https://dl.acm.org/doi/pdf/10.1145/3372224.3419982)

## Citing

We kindly ask scientific works and studies that make use of the repository to cite it in their associated publications. Similarly, we ask open-source and closed-source works that make use of the repository to warn us about this use.

You can cite our work using the following BibTeX entry:

    @phdthesis{assana2020cardiovascular,
      title={Cardiovascular activity monitoring using mmWaves},
      author={Assana, Salah},
      year={2020},
      school={Massachusetts Institute of Technology}
    }
