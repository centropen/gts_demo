#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/move/move.hpp>

/**
 * Thread safe msg UUID generator
 */
namespace gts {
	namespace msg {

#define GTS_MSG_UUID_C gts::msg::UUID()()->getC();
#define GTS_MSG_UUID_S gts::msg::UUID()()->getS();

	struct UUID {
		explicit UUID() : _uuid(boost::uuids::random_generator()()) {}
		UUID* operator()() { return this; }

		const std::string getS() const {
			return ((boost::lexical_cast
				<std::string>(_uuid)));
		}
		const char* getC() const {
			return this->getS().c_str();
		}

private:
	boost::uuids::uuid _uuid;
};

	}

} //gts
