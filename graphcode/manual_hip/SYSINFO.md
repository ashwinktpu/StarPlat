# System Information of machines used for testing

The following table contains relevent information of various machines used for testing algorithms implemented using AMD HIP Code.

| Parameter     | AMD Machine<br>(rnamdgpu) | AMD HPC           |   AWS             |   Aqua    |
|-----------    |-------------              |---------          |-----------        |-----------|
|**OS**         |Ubuntu 22.04.2 LTS         |Rocky Linux 9.1    |Ubuntu 22.04.2 LTS |RHEL Server 7.6 (Maipo)|
|**Linux Kernel**|5.19.0-45-generic         |5.14.0-162.18.1.el9_1.x86_64|5.19.0-1027-aws|3.10.0-957.el7.x86_64|
|**hip**        |5.4.22804-474e8620         |5.4.22803-474e8620 |5.4.22804-474e8620 |None       |
|**cuda**       |None                       |None               |cuda_11.5.r11.5<br>compiler.30672275_0|release 10.1<br>V10.1.243|
|**GPU**        |AMD Radeon RX 6900 XT      |AMD Arcturus GL-XL [Instinct MI100]|Nvidia Tesla T4 TU104GL|V100           |
|**Multi-GPU**  |Yes, but only one detected |Yes, 4             |No                 |Yes, 2           |
|**Driver**     |5.4.50406-1588631.20.04    | -                 |nvidia-driver-530  | CUDA 8,9,10,11,12          |
|**Processing Units per GPU**|80 CUs        |120 CUs            |2560 CUDA Cores    | 5120 Cores (80 SMs)          |
|**CPU**        |Intel(R) Core(TM) i9-10980XE CPU @ 3.00GHz|AMD EPYC 7V13 64-Core Processor (x2)|Intel(R) Xeon(R) Platinum 8259CL CPU @ 2.50GHz|Intel(R) Xeon(R) Gold 6248 CPU @ 2.50GHz|
