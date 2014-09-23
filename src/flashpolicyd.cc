#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <brickred/command_line_option.h>
#include <brickred/dynamic_buffer.h>
#include <brickred/io_service.h>
#include <brickred/socket_address.h>
#include <brickred/tcp_service.h>
#include <brickred/unix/system.h>

using namespace brickred;

class PolicyServer {
public:
    PolicyServer() : tcp_service_(io_service_)
    {
        tcp_service_.setRecvMessageCallback(BRICKRED_BIND_MEM_FUNC(
            &PolicyServer::onRecvMessage, this));
        tcp_service_.setPeerCloseCallback(BRICKRED_BIND_MEM_FUNC(
            &PolicyServer::onPeerClose, this));
        tcp_service_.setErrorCallback(BRICKRED_BIND_MEM_FUNC(
            &PolicyServer::onError, this));
    }

    ~PolicyServer()
    {
    }

    bool init(const SocketAddress &addr)
    {
        if (tcp_service_.listen(addr) < 0) {
            ::fprintf(stderr, "socket listen failed: %s\n",
                      ::strerror(errno));
            return false;
        }

        return true;
    }

    void run()
    {
        io_service_.loop();
    }

    void onRecvMessage(TcpService *service,
                       TcpService::SocketId socket_id,
                       DynamicBuffer *buffer)
    {
        static char policy_file[] =
            "<?xml version=\"1.0\"?>"
            "<cross-domain-policy>"
                "<allow-access-from domain=\"*\" to-ports=\"*\"/>"
            "</cross-domain-policy>";

        std::string buffer_string(buffer->readBegin(),
                                  buffer->readableBytes());
        buffer->read(buffer->readableBytes());

        if (buffer_string.find("<policy-file-request/>") !=
            std::string::npos) {
            service->sendMessageThenClose(socket_id,
                policy_file, sizeof(policy_file) - 1);
        } else {
            service->closeSocket(socket_id);
        }
    }

    void onPeerClose(TcpService *service,
                     TcpService::SocketId socket_id)
    {
        service->closeSocket(socket_id);
    }

    void onError(TcpService *service,
                 TcpService::SocketId socket_id,
                 int error)
    {
        service->closeSocket(socket_id);
    }

private:
    IOService io_service_;
    TcpService tcp_service_;
};

static void printUsage(const char *progname)
{
    ::fprintf(stderr, "usage: %s\n"
              "[-l <ip>]\n"
              "[-p <port>]\n"
              "[--pidfile <pid_file>]\n",
              progname);
}

int main(int argc, char *argv[])
{
    std::string ip = "0.0.0.0";
    uint16_t port = 843;
    std::string pid_file = "/tmp/flashpolicyd.pid";

    CommandLineOption options;
    options.addOption("l", CommandLineOption::ParameterType::REQUIRED);
    options.addOption("p", CommandLineOption::ParameterType::REQUIRED);
    options.addOption("pidfile", CommandLineOption::ParameterType::REQUIRED);

    if (options.parse(argc, argv) == false) {
        printUsage(argv[0]);
        return -1;
    }

    if (options.hasOption("l")) {
        ip = options.getParameter("l");
    }
    if (options.hasOption("p")) {
        port = ::atoi(options.getParameter("p").c_str());
    }
    if (options.hasOption("pidfile")) {
        pid_file = options.getParameter("pidfile");
    }

    PolicyServer server;
    if (server.init(SocketAddress(ip, port)) == false) {
        return -1;
    }

    if (os::daemon() == false) {
        ::fprintf(stderr, "daemon failed\n");
        return -1;
    }

    if (os::createPidFile(pid_file.c_str()) == false) {
        ::fprintf(stderr, "create pid file failed\n");
        return -1;
    }

    server.run();

    return 0;
}
