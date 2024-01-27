#ifndef __GPIO_HPP__
#define __GPIO_HPP__


#define HIGH true
#define LOW false

//AbstractGPIO interface
struct AbstractGPIO {
    virtual void High(void) = 0;

    virtual void Low(void) = 0;

    virtual void Toggle(void) = 0;

    virtual bool getState(void) = 0;
};

#endif /* __GPIO_HPP__ */