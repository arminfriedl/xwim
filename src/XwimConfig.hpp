#pragma once

namespace xwim {

enum class Action { COMPRESS, EXTRACT };

class XwimConfig {

public:
  Action get_action();


}

}
