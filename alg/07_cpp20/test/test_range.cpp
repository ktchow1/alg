#include<iostream>
#include<cassert>
#include<vector>
#include<ranges>

// *** alg *** //
#include<range.h>
#include<utility.h>

void print(auto& vec, auto& std_view, auto& alg_view)
{
    const bool enabled = false;
    if (enabled)
    {
        std::cout << "\nvec = "; for(const auto& x:vec)      std::cout << x << " ";
        std::cout << "\nstd = "; for(const auto& x:std_view) std::cout << x << " ";
        std::cout << "\nalg = "; for(const auto& x:alg_view) std::cout << x << " ";
    }
}


void test_range_impl()
{
    std::uint32_t size = 50 + rand()%50;
    std::uint32_t min  = 0;
    std::uint32_t max  = 100;
    auto vec = gen_random_vec<std::uint32_t>(size, min, max);


    // ************************* //
    // filter > transform > take //
    // ************************* //
    {
        auto std_view = vec | std::views::filter   ([](const auto& x) { return x%2==1; } )
                            | std::views::transform([](const auto& x) { return x*2;    } ) | std::views::take(15);
        auto alg_view = vec | alg::       filter   ([](const auto& x) { return x%2==1; } )
                            | alg::       transform([](const auto& x) { return x*2;    } ) | alg::take(15); 

        assert(compare<std::uint32_t>(std_view, alg_view));
        print(vec, std_view, alg_view);
    }


    // ************************* //
    // transform > take > filter //
    // ************************* //
    {
        auto std_view = vec | std::views::transform([](const auto& x) { return x+2;    } ) | std::views::take(32)
                            | std::views::filter   ([](const auto& x) { return x%3==1; } );
        auto alg_view = vec | alg::       transform([](const auto& x) { return x+2;    } ) | alg::take(32)
                            | alg::       filter   ([](const auto& x) { return x%3==1; } );

        assert(compare<std::uint32_t>(std_view, alg_view));
        print(vec, std_view, alg_view);
    }


    // ************************* //
    // take > filter > transform //
    // ************************* //
    {
        auto std_view = vec | std::views::take(40)
                            | std::views::filter   ([](const auto& x) { return x%3>=1; } )
                            | std::views::transform([](const auto& x) { return x+20;   } );
        auto alg_view = vec | alg::take(40)
                            | alg::filter   ([](const auto& x) { return x%3>=1; } )
                            | alg::transform([](const auto& x) { return x+20;   } ); 

        assert(compare<std::uint32_t>(std_view, alg_view));
        print(vec, std_view, alg_view);
    }


    // ******************************************* //
    // filter > transform > take > tranform > take //
    // ******************************************* //
    {
        auto std_view = vec | std::views::filter   ([](const auto& x) { return x%2==1; } )
                            | std::views::transform([](const auto& x) { return x*2;    } ) | std::views::take(20)
                            | std::views::transform([](const auto& x) { return x+2;    } ) | std::views::take(10);
        auto alg_view = vec | alg::       filter   ([](const auto& x) { return x%2==1; } )
                            | alg::       transform([](const auto& x) { return x*2;    } ) | alg::take(20) 
                            | alg::       transform([](const auto& x) { return x+2;    } ) | alg::take(10); 

        assert(compare<std::uint32_t>(std_view, alg_view));
        print(vec, std_view, alg_view);
    }


    // *********************************************** //
    // filter > transform > filter > tranform > filter //
    // *********************************************** //
    {
        auto std_view = vec | std::views::filter   ([](const auto& x) { return x%3>=1; } )
                            | std::views::transform([](const auto& x) { return x+10;   } )
                            | std::views::filter   ([](const auto& x) { return x%3==0; } )
                            | std::views::transform([](const auto& x) { return x+2;    } )
                            | std::views::filter   ([](const auto& x) { return x%2==0; } );
        auto alg_view = vec | alg::       filter   ([](const auto& x) { return x%3>=1; } )
                            | alg::       transform([](const auto& x) { return x+10;   } )
                            | alg::       filter   ([](const auto& x) { return x%3==0; } )
                            | alg::       transform([](const auto& x) { return x+2;    } )
                            | alg::       filter   ([](const auto& x) { return x%2==0; } );

        assert(compare<std::uint32_t>(std_view, alg_view));
        print(vec, std_view, alg_view);
    }

}


void test_range()
{
    for(std::uint32_t n=0; n!=20; ++n)
    {
        test_range_impl();
    }
    print_summary("range library", "succeeded");
}


