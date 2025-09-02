idf_component_register(SRCS "PIDCtrl.c"
                    INCLUDE_DIRS "include"
                    REQUIRES ADCDetect PWMCtrl UIControl espressif__pid_ctrl)
