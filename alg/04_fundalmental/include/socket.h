
// ****************** //
// *** Boost ASIO *** //
// ****************** //
/*  Plesae determine the prototype in sequence of (1), (2) and (3)

                    |     constructor                    function argument
    ----------------+-----------------------------------------------------------
    deadline_timer  |     io_service, interval
    tcp::acceptor   |     io_service, port
    tcp::socket     |     io_service
    ----------------+-----------------------------------------------------------
    async_wait      |                                (1) callback     
    async_accept    |                                    callback, socket
    asycn_read_some |                                    callback, buffer
    ----------------+-----------------------------------------------------------
    callback        | (3) deadline_timer             (2) error_placeholder
    callback        |     tcp::async_accept, socket      error_placeholder
    callback        |     tcp::socket, buffer            error_placeholder, size
*/



class deadline_timer_callback
{
public:
    // (3) callback constructor
    deadline_timer_callback(deadline_timer& _timer, std::uint32_t _count) : timer(_timer), count(_count)
    {
    }
    
    // (2) callback function argument
    void fct(boost::placeholders::error)
    {
        --count;
        if (count==0) return;

        // (1) async function argument 
        timer.async_wait(std::bind(&deadline_timer_callback::fct, this));
    }

private:
    deadline_timer& timer;
    std::uint32_t& count;
};

void run_deadline_timer()
{
    boost::asio::io_service io;
    boost::asio::deadline_timer timer(io, boost::posic_time::seconds(1));
    deadline_timer_callback cb(timer, 100);
    timer.async_wait(std::bind(&deadline_timer_callback::fct, std::ref(cb)));
    io.run();
}

class tcp_server
{
public:
    tcp_server(boost::asio::io_service& _io, std::uint32_t port) : io(_io), acceptor(_io, port)
    {
    }

    void async_run()
    {
        socket = std::make_shared<boost::asio::tcp::socket>(io);
        acceptor->async_wait(*socket, std::bind(&tcp_server::callback, this, boost::placeholders::error));
    }

private:
    void callback(boost::placeholders::error& err)
    {
        socket->set_option(boost::asio::tcp::no_delay);
        auto session = std::make_shared<tcp_session>(socket); 
        session.async_run();
        sessions.insert(session);
        async_run();
    }

private:
    boost::asio::io_service& io;
    boost::asio::tcp::acceptor acceptor;
    std::shared_ptr<boost::asio::tcp::socket> socket;
    std::set<std::shared_ptr<tcp_sessions>> sessions;
};

class tcp_session
{
public:
    tcp_session(std::shared_ptr<boost::asio::tcp::socket>& _socket) : socket(_socket)
    {
    }

    void async_run()
    {
        socket->async_run
        (
            boost::asio::buffer(str),
            std::bind(&tcp_session::callback, this, boost::placeholders::error, boost::placeholders::_1);
        );
    }

private:
    void callback(boost::placeholders::error& err, std::uint32_t size)
    {
        std::cout << str.substr(0,size);
        async_run();
    }

private:
    std::shared_ptr<boost::asio::tcp::socket> socket;
    std::string str;
};

class tcp_client
{
public:
    tcp_client(boost::asio::io_service& _io) : io(_io), socket(_io)
    {
    }

    bool connect(const std::string& ip, std::uint32_t port)
    {
        boost::asio::tcp::resolver resolver(io);
        boost::asio::tcp::query query(io, port);
        boost::asio::tcp::endpoint endpoint = resolver.resolve(query);
        socket.connect(endpoint);
        socket.set_option(boost::asio::tcp::no_delay);
    }

private:
    boost::asio::io_service& io;
    boost::asio::tcp::socket socket;
};
