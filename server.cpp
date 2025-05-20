#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>

using namespace boost::asio;
using namespace std;

class ServerTcp
{
private:
	vector<shared_ptr<ip::tcp::socket>> client;
	mutex client_mutex;

	io_context io;
	ip::tcp::acceptor acceptor;

	void acceptClient()
	{
		thread([this]()
			{
				while (true)
				{
					auto socket = make_shared<ip::tcp::socket>(io);
					acceptor.accept(*socket);

					{
						lock_guard<mutex> lock(client_mutex);
						client.push_back(socket);
					}

					thread(&ServerTcp::handle_client, this, socket).detach();
				}
			}).detach();
	}

	void handle_client(shared_ptr<ip::tcp::socket> sock)
	{
		try
		{
			char data[2048];
			while (true)
			{
				memset(data, 0, sizeof(data));

				size_t len = sock->read_some(buffer(data));
				if (len == 0) break;

				string msg(data, len);

				cout << "[adress]: " << sock->remote_endpoint().address().to_string()
					<< " [msg client]:  " << msg << '\n';

				lock_guard<mutex> lock(client_mutex);
				for (auto& cln : client)
				{
					if (cln != sock)
					{
						boost::system::error_code ec;
						write(*cln, buffer(msg), ec);
					}
				}
			}
		}
		catch (exception& e)
		{
			cout << "[client off]" << e.what() << '\n';
			lock_guard<mutex> lock(client_mutex);
			client.erase(std::remove(client.begin(), client.end(), sock), client.end());
		}
	}
public:

	ServerTcp(short port) : acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), port))
	{
		acceptClient();
	}

	void run()
	{
		io.run();
	}
	
};

int main()
{

	try
	{
		ServerTcp obj(5000);
		cout << "[Server started]\n";
		while (true) this_thread::sleep_for(std::chrono::seconds(1));
	}
	catch (const exception& e)
	{
		cout << "[{error Server}]" << e.what() << '\n';
	}
}


