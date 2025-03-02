#include "Directives.hpp"

ServerName::ServerName(t_context context) : _value()
{
	_context = context;
}

void	ServerName::action(const std::string &value, t_context context)
{
	if (_context != GLOBAL)
	{
		if (_context != context)
			throw badContext();
	}
	std::vector<std::string> temp;
	temp = split(value, SPACES);
	_value.insert(_value.end(), temp.begin(), temp.end());
}

void	ServerName::print_content() const
{
	std::cout << "\nServer name: \n\n";
	for (size_t j = 0; j < _value.size(); j++)
		std::cout << "\t" << _value[j] << std::endl;
	std::cout << "\n";
}

const std::string	&ServerName::get_value(size_t i) const
{
	return (_value[i]);
}


Root::Root(t_context context) : _value("html")
{
	_context = context;
}

void	Root::action(const std::string &value, t_context context)
{
	if (_context != GLOBAL)
	{
		if (_context != context)
			throw badContext();
	}
	std::vector<std::string> temp;
	temp = split(value, SPACES);
	if (temp.size() > 1)
		throw TooManyRoot();
	_value = temp[0];
}

void	Root::print_content() const
{
	std::cout << "\nroot: " << _value << std::endl;
}

const std::string	&Root::get_value() const
{
	return (_value);
}

ClientMaxBodySize::ClientMaxBodySize(t_context context) : _value(1024) // 1mb?
{
	_context = context;
}

void	ClientMaxBodySize::action(const std::string &value, t_context context)
{
	if (_context != GLOBAL)
	{
		if (_context != context)
			throw badContext();
	}

	size_t pos = 0;

	while (pos < value.length())
	{
		if (isdigit(value[pos]))
			break ;
		pos++;
	}

	int len = pos;
	while (isdigit(value[len]))
		len++;

	_value = atoi(value.c_str());
}

void	ClientMaxBodySize::print_content() const
{
	std::cout << "\b\nclient_max_body_size: " << _value << std::endl;
}


ErrorPage::ErrorPage(t_context context) : _value()
{
	_context = context;
}

/**
 * 
 * ERROR_PAGE SHOULD BE: 
 * 	 [pair<[500, 300, 200], "url">],
 *   [pair<[400, 300, 200], "url2">]]
 */
void	ErrorPage::action(const std::string &value, t_context context)
{
	if (_context != GLOBAL)
	{
		if (_context != context)
			throw badContext();
	}

	std::pair<std::vector<int>, std::string>	ret;
	std::stringstream							ss;
	size_t	i = 0;
	int		error;

	std::vector<std::string> splited = split(value, SPACES);
	if (splited.size() == 1)
		throw ErrorPageOnlyOneArg();

	for (; i < splited.size() - 1; i++)
	{
		for (size_t j = 0; j < splited[i].size(); j++)
		{
			if (!isdigit(splited[i][j]))
				throw ErrorPageCodeNotInt();
		}

		ss << splited[i];
		ss >> error;
		ss.clear();

		if (error < 100 || error > 599)
			throw ErrorPageCodeNotKnowed();

		ret.first.push_back(error);
	}
	ret.second = splited[i];
	_value.push_back(ret);
}

void	ErrorPage::print_content() const
{
	std::cout << "\nError page: \n\n";
	for (size_t j = 0; j < _value.size(); j++)
	{
		for (size_t x = 0; x < _value[j].first.size(); x++)
			std::cout << "\terror number - " << _value[j].first[x] << std::endl;
		std::cout << "\terror url - " << _value[j].second << std::endl;
		std::cout << "\t-----------------\n";
	}
}

/**
 * 
 * Listen Examples:
 * 		
 * 		listen 127.0.0.1:8000;
 * 		listen 127.0.0.1;
 * 		listen 8000;
 * 		listen *:8000;
 * 		listen localhost:8000;
 */ 

Listen::Listen(t_context context) // Not done
{
	_context = context;
}

/**
 * Existem 3 possibilidades:
 * 
 * 1. 80
 * 2. :80
 * 
 * 3. 127.3.3.9:80
 * 
 * 1 e 2 é suposto ser a mesma coisa, mas não posso apagar o ';'  porque pode dar merda, neste caso 1127.28.9.2:80
 * 
 */
void	Listen::action(const std::string &value, t_context context)
{
	if (_context != GLOBAL)
	{
		if (_context != context)
			throw badContext();
	}
	// std::cout << "value is: " << value << std::endl;
	_l.push_back(new ListenIndv(split(value, SPACES)));
	std::cout << _l.size() << std::endl;

}

void	Listen::check_dup_listen_directives()
{
	std::cout << _l.size() << std::endl;
	for (size_t i = 0; i < _l.size(); i++)
	{
		const std::pair<std::string, int>	temp = _l[i]->get_value();
		for (size_t x = i + 1; x < _l.size(); x++)
		{
			std::cout << i << std::endl;
			const std::pair<std::string, int>	temp2 = _l[x]->get_value();
			if (temp.first == temp2.first && temp.first == temp2.first)
			{
				std::cout << "ListenBlockEqual\n";
				throw ListenBlockEqual();
			}
		}
	}
}

void	Listen::print_content() const {}


ListenIndv::ListenIndv(const std::vector<std::string> &split)
{
	_spl_value = split;
	std::string value = split[0]; 
	std::vector<std::string>	_options;
	
	size_t f_ret = value.find(':');

	_syntax = ADDRESS_PORT;
	// No caso 80 e :80
	if (f_ret == std::string::npos || f_ret == 0)
	{
		if (value.find('.') != std::string::npos)
			_syntax = ADDRESS;
		else
			_syntax = PORT;
	}

	if (_syntax == ADDRESS_PORT)
	{
		handle_address(trim(value.substr(0, f_ret), SPACES));
		handle_port(value.substr(f_ret + 1));
	}
	else if (_syntax == ADDRESS)
	{
		_port = 80;
		handle_address(trim(value, SPACES));
	}
	else
	{
		_address = "localhost";
		if (value[0] == ':')
			value.erase(0, 1);
		handle_port(value.c_str());
	}
	std::cout << "Port is: " << _port << std::endl;
	std::cout << "Address is: " << _address << std::endl;
	_value = std::make_pair(_address, _port);

}

ListenIndv *Listen::get_listen_indv(size_t i) const
{
	std::cout << _l.size() << std::endl;
	std::cout << i << std::endl;

	if (i >= _l.size())
		return (NULL);
	return (_l[i]);
}


void	ListenIndv::handle_address(std::string addr)
{

	if (addr == "localhost" || addr == "127.0.0.1")
	{
		_address = "localhost";
		return ;
	}
	
	if (addr.find_last_not_of("0123456789.") != std::string::npos)
		throw ListenAddressNotValid();
	else if (count_char(addr, '.') != 3)
		throw ListenAddressNotValid();

	_address = addr;

	while (1)
	{
		int num = convert_to<int, std::string>(addr);
		addr.erase(0, convert_to<std::string>(num).size());

		if (num < 0 || num > 255)
			throw ListenAddressNotValid();
		
		if (addr.length() == 0)
			break ;

		addr.erase(0, 1);

		if (!isdigit(addr[0]))
			throw ListenAddressNotValid();
	}
}

void	ListenIndv::handle_port(const std::string &port)
{
	std::cout << "port is: " << port << std::endl;
	if (port.find_last_not_of("0123456789") != std::string::npos)
		throw ListenPortNotValid();

	_port = convert_to<int>(port);
	if (_port < 0 || _port > 65535)
		throw ListenPortNotValid();
}

const std::pair<std::string, int>	&ListenIndv::get_value() const
{
	return (_value);
}


LimitExcept::LimitExcept(t_context context)
{
	_context = context;
	for (int i = 0; i < 3; i++)
		_value[i] = 0;
}

void	LimitExcept::action(const std::string &value, t_context context)
{
	std::cout << "LimitExcept context is: " << _context << std::endl;
	if (_context != GLOBAL)
	{
		if (_context != context)
			throw badContext();
	}

	if (((int)value.find("GET") != -1))
		_value[0] = 2;
	if (((int)value.find("POST") != -1))
		_value[1] = 2;	
	if (((int)value.find("DELETE") != -1))
		_value[2] = 2;

	if (((int)value.find("deny") != -1))
	{
		for (int i = 0; i < 3; i++) 
		{ 
			if (_value[i] == 0)
				_value[i] = 1;
			
		}
	}
}

void	LimitExcept::print_content() const
{
	std::cout << "limit except:  \n\n\tGET - " << _value[0] 
	<< "\n\tPOST - " << _value[1]
	<< "\n\tDELETE - " << _value[2] << std::endl;
}


Index::Index(t_context context) : _value()
{
	_context = context;
}

void	Index::action(const std::string &value, t_context context)
{
	if (_context != GLOBAL)
	{
		if (_context != context)
			throw badContext();
	}

	std::vector<std::string> temp;
	temp = split(value, SPACES);
	_value.insert(_value.end(), temp.begin(), temp.end());
}

void	Index::print_content() const
{
	std::cout << "Index: \n\n";
	for (size_t i = 0; i < _value.size(); i++)
		std::cout << "\t" << _value[i] << std::endl;
	std::cout << "\n";
}

const std::vector<std::string>	&Index::get_value() const
{
	return (_value);
}
