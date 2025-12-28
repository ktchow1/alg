#include <iostream>
#include <exception>
#include <cassert>
#include <bitset>
#include <thread>
#include <mutex>
#include <unistd.h>

#include <apt.h>
#include <utility.h>


// ****************************** //
// *** Question 2 - brainfuck *** //
// ****************************** //
namespace apt
{
    void test_question2()
    {
        brainfuck bf(8);

        // program 1 - print a fixed number 64
        {
            std::string code = "++++++++[>++++++++<-]>.";
            auto status = bf.compile(code);
            auto output = bf.run(""); 

            assert(output == "@");
            assert(output.size() == 1);
            assert(static_cast<std::uint32_t>(static_cast<unsigned char>(output[0])) == 64);
        }

        // program 2 - print Hello world 
        {                                                           // code from ChatGPT, can be optimized
            std::string code = "++++++++++"                         // scale the following by 10
                               "[>+++++++>++++++++++>+++>+<<<<-]"   // data[1] =   7, data[2] = 10, data[3] = 3, data[4] = 1
                               ">++."                               // data[1] =  72 = "H"
                               ">+."                                // data[2] = 101 = "e"
                               "+++++++.."                          // data[2] = 108 = "ll"
                               "+++."                               // data[2] = 111 = "o"
                               ">++."                               // data[3] =  32 = " "
                               "<<+++++++++++++++."                 // data[1] =  87 = "W"
                               ">."                                 // data[2] = 111 = "o"
                               "+++."                               // data[2] = 114 = "r"
                               "------."                            // data[2] = 108 = "l"
                               "--------."                          // data[2] = 100 = "d"
                               ">+.";                               // data[3] =  33 = "!"

            auto status = bf.compile(code);
            auto output = bf.run(""); 
            assert(output == "Hello World!");
        }

        // program 3 - print input until EOF (i.e. null character)
        {
            std::string code = ",[.,]";
            auto status = bf.compile(code);

            // Remark 1 : compile once, run many
            // Remark 2 : there is null char (EOF) at the end of string
            auto output0 = bf.run("ABCDEFGHIJ"); 
            assert(output0 == "ABCDEFGHIJ");
            auto output1 = bf.run("ABC....HIJ"); 
            assert(output1 == "ABC....HIJ");
            auto output2 = bf.run(" ABCD$%^ \n"); 
            assert(output2 == " ABCD$%^ \n");
            auto output3 = bf.run(""); 
            assert(output3 == "");
        }

        // program 4 - copy from a memory to another memory
        {
            std::string code = ",          "    // input to data[0]
                               "[->+>+<<]  "    //  copy to data[1] and data[2]
                               ">>[-<<+>>] "    //  move    data[2] to  data[0]
                               "<<.>.      ";   // print    data[0] and data[1]    

            auto status = bf.compile(code);
            auto output = bf.run("a"); 
            assert(output == "aa");
        }

        // program 5 - add two numbers from input 
        {
            std::string code = ",>,<[->+<]>.";
            auto status = bf.compile(code);

            std::string input0;
            input0.push_back(static_cast<char>(4));
            input0.push_back(static_cast<char>(7));
            auto output0 = bf.run(input0);

            std::string input1;
            input1.push_back(static_cast<char>(15));
            input1.push_back(static_cast<char>(28));
            auto output1 = bf.run(input1);

            std::string input2;
            input2.push_back(static_cast<char>(36));
            input2.push_back(static_cast<char>(69));
            auto output2 = bf.run(input2);

            assert(output0.size() == 1 && static_cast<std::uint32_t>(static_cast<unsigned char>(output0[0])) ==  11);
            assert(output1.size() == 1 && static_cast<std::uint32_t>(static_cast<unsigned char>(output1[0])) ==  43);
            assert(output2.size() == 1 && static_cast<std::uint32_t>(static_cast<unsigned char>(output2[0])) == 105);
        }
    
        // program 6 - multiply two numbers from input
        {
            std::string code = ",>,<           "   // input to data[0] and data[1]              
                               "[              "   // loop  on data[0]
                               "    >[->+>+<<] "   // copy  data[1] to data[2] and data[3]
                               "    >>[-<<+>>] "   // move  data[3] to data[1]
                               "    <[->>+<<]  "   // add   data[2] to data[4] (data[4] is accumulator, at this point data[2] and data[3] are zero)
                               "    <<-        "   // dec   data[0]
                               "]              "
                               ">>>>.          ";  // print data[4]

            auto status = bf.compile(code);

            std::string input0;
            input0.push_back(static_cast<char>(4));
            input0.push_back(static_cast<char>(7));
            auto output0 = bf.run(input0);
  
            std::string input1;
            input1.push_back(static_cast<char>(10));
            input1.push_back(static_cast<char>(10));
            auto output1 = bf.run(input1);

            std::string input2;
            input2.push_back(static_cast<char>(15));
            input2.push_back(static_cast<char>(11));
            auto output2 = bf.run(input2); 

            // ************************************************** //
            // In some platforms, char is considered signed char, //
            // hence need 2 static_cast to get the correct value, //
            // otherwise it returns wrong value for char >= 128.  //
            // ************************************************** //
            assert(output0.size() == 1 && static_cast<std::uint32_t>(static_cast<unsigned char>(output0[0])) ==  28);
            assert(output1.size() == 1 && static_cast<std::uint32_t>(static_cast<unsigned char>(output1[0])) == 100);
            assert(output2.size() == 1 && static_cast<std::uint32_t>(static_cast<unsigned char>(output2[0])) == 165);
        } 
  
        print_summary("apt : question 2", "succeeded");
    }
}


// ************************** //
// *** Question 3 - const *** //
// ************************** //
namespace apt
{
    struct A 
    {
        static std::uint32_t x;
        void fct() const   { x = 1; }
        void fct()         { x = 2; }
    };
    std::uint32_t A::x = 0;

    void fct(const A& a)   { a.fct(); }
    void fct(A& a)         { a.fct(); }

    void test_question3() 
    {
        const A a0;
        A a1;

        a0.fct();
        assert(A::x == 1);
        a1.fct();
        assert(A::x == 2);
        fct(a0);
        assert(A::x == 1);
        fct(a1);
        assert(A::x == 2);
        
        print_summary("apt : question 3", "succeeded");
    }
}


// ********************************************* //
// *** Question 4 - floating point precision *** //
// ********************************************* //
// For convenience, lets call :
// * number that are   precisely represented by "normalized binary scientific notation" as   precise-number
// * number that are imprecisely represented by "normalized binary scientific notation" as imprecise-number
//
// Concepts for this question :
// * Operation between   2   precise-numbers  will give   precise-output (unless it overflows).
// * Operation involve 1/2 imprecise-numbers "may" give imprecise-output.
// * Two imprecise-outputs can be identical if they are generated in same way.
//
namespace apt
{
    void test_question4() 
    {
        // 1.2 is imprecise,        8.0 is precise, imprecise / precise gives imprecise
        // 1.2 is imprecise, 32.0/256.0 is precise, imprecise * precise gives imprecise, but identical to 1.2/8.0
        {
            double x = 1.2 / 8.0;             
            double y = 1.2 * (32.0 / 256.0);  
            assert(x == y); 
        }
        
        // 1.0 is precise,        7.0 is imprecise, precise / imprecise gives imprecise
        // 1.0 is precise, 24.0/140.0 is imprecise, precise * imprecise gives imprecise, but may/may NOT identical to 1.0/7.0
        {
            double x = 1.0 / 7.0;
            double y = 1.0 * (20.0 / 140.0); 
            assert(x == y); 
        }{
            double x = 1.0 / 7.0;
            double y = 1.0 * (20.1 / 140.7); 
            assert(x != y); 
        }{
            double x = 1.0 / 7.0;
            double y = 1.0 * (20.2 / 141.4);
            assert(x == y); 
        }{
            double x = 1.0 / 7.0;
            double y = 1.0 * (20.3 / 142.1);
            assert(x != y); 
        }{
            double x = 1.0 / 7.0;
            double y = 1.0 * (20.4 / 142.8);
            assert(x == y); 
        }{
            double x = 1.0 / 7.0;
            double y = 1.0 * (20.5 / 143.5);
            assert(x == y); 
        }

        print_summary("apt : question 4", "succeeded");
    }


    // ********************************************************************* //
    // Double is represented as "normalized binary scientific notation".
    //
    // bit  0       = sign (0 for positive)
    // bit  1-11    = exponential 
    // bit 12-63    = fraction (mantissa)
    // double value = (-1)^sign * (1.mantissa) * 2^(expoenential-1023)
    //
    // Why do we subtract 1023 in exponential?
    // * exponential takes value from (-2^10)+1 to (+2^10), by shifting 1023
    // ********************************************************************* //
    // Example 
    //
    // 10.375 = 8 + 2 + 1/4 + 1/8
    //        = 2^3 + 2^1 + 2^(-2) + 2^(-3)
    //        = 1010.011(base 2) 
    //        = 1.010011(base 2) * 2^3
    // sign   = 0
    // exponential = 3 = 11(base 2)
    // mantissa    = 010011(base 2) <--- remove the preceeding 1.0
    // ********************************************************************* //
    void test_question4_double_representation() 
    {
        double d = 10.375;
        std::uint64_t i = *reinterpret_cast<const std::uint64_t*>(&d);

        assert((std::uint64_t)  (i >> 63 & 0x1  ) == 0);
        assert((std::uint64_t)  (i >> 52 & 0x7FF) == 1023 + 3);
        std::bitset<52> mantissa(i & 0xFFFFFFFFFFFFF); // There are 13 'F's.
  
        assert(mantissa[51] == 0);
        assert(mantissa[50] == 1);
        assert(mantissa[49] == 0);
        assert(mantissa[48] == 0);
        assert(mantissa[47] == 1);
        assert(mantissa[46] == 1); 
        for(std::uint64_t n=0; n!=46; ++n)
        {
            assert(mantissa[6] == 0);
        }
        print_summary("apt : question 4 (double representation)", "succeeded");
    }
}


// ****************** //
// *** Question 5 *** //
// ****************** //
// Fact 1, main thread cannot catch exception from other threads : 
// * each thread has its own stack
// * caught execption thrown from thread A triggered stack-unwind in thread A 
// * caught execption thrown from thread B triggered stack-unwind in thread B 
//
// Fact 2, when exception thrown by thread A is not caught by thread A :
// * it will invoke std::terminate() 
// * other threads cannot invoke std::terminate()
// * other threads will be terminated, but "where they stop" is a race condition
//
// There are 4 possibilities for this question : 
// * thread A throws exception, thread A invokes std::terminate()
// * thread B throws exception, thread B invokes std::terminate()
// * both thread A and B throw, thread A invokes std::terminate()
// * both thread A and B throw, thread B invokes std::terminate()
//
namespace apt
{
    std::mutex mutexA;
    std::mutex mutexB;

    void threadA()
    {
        mutexA.lock();
        throw std::runtime_error("Exception from ThreadA");
        mutexB.lock();
        mutexA.unlock();
        mutexB.unlock(); 
    }

    void threadB()
    {
        mutexB.lock();
        throw std::runtime_error("Exception from ThreadB");
        mutexA.lock();
        mutexB.unlock();
        mutexA.unlock(); 
    }

    void test_question5()
    {
        try
        {
            std::thread tA(threadA);
            std::thread tB(threadB);
            tA.join();
            tB.join();
        } 
        catch (const std::exception& e) 
        {
        }
    }
}


void test_apt()
{
    apt::test_question2();
    apt::test_question3();
    apt::test_question4();
    apt::test_question4_double_representation();

    // To run test_question5, please :
    // 1. comment out all other tests
    // 2. run repeatedly to see all possible race conditions
    //
//  apt::test_question5();
}
