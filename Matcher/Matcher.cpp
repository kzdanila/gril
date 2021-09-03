#include "Matcher.h"

Matcher::Matcher() : m_endFlag1(false), m_endFlag2(false), m_reciever1(Reciever(0)), m_reciever2(Reciever(1))
{

}

uint32_t last = 0;

void Matcher::CalculateRoutinine()
{
	while (!m_endFlag1.load() && !m_endFlag2.load())
	{
		std::unique_lock ul(m_recvLocker);
		m_cv.wait(ul,
			[&] 
			{ 
				auto it = m_currentEpochMap.begin();
				while (it != m_currentEpochMap.end())
				{
					if (it->second.isReady())
					{
						std::cout << "Epoch=" << it->first << " Distance=" << Distance(it->second) << std::endl;
						it = m_currentEpochMap.erase(it);
					}
					else
					{
						it++;
					}
				}
				return m_currentEpochMap.empty();
			}
		);	
		ul.unlock();
		m_cv.notify_all();
	}
}


void Matcher::InitRecivers()
{
	std::filesystem::path path = "../src/DRC1-0805a.tps";
	m_firstReaderThread = std::thread(&Reciever::Recv, m_reciever1, path, std::ref(m_endFlag1),
		std::ref(m_cv), std::ref(m_recvLocker), std::ref(m_currentEpochMap));

	std::filesystem::path path2 = "../src/DRD1-0805a.tps";
	m_secondReaderThread = std::thread(&Reciever::Recv, m_reciever2, path2, std::ref(m_endFlag2),
		std::ref(m_cv), std::ref(m_recvLocker), std::ref(m_currentEpochMap));

	m_computeThread = std::thread(&Matcher::CalculateRoutinine, this);
}

Matcher::~Matcher()
{
	if(m_firstReaderThread.joinable())
	    m_firstReaderThread.join();
	//std::cout << "join" << std::endl;
	if (m_secondReaderThread.joinable())
	    m_secondReaderThread.join();
	//std::cout << "2d join" << std::endl;
	if (m_computeThread.joinable())
	    m_computeThread.join();
	//std::cout << "comp join" << std::endl;
}

