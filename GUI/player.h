#ifndef PLAYER_H
#define PLAYER_H

namespace GEMUFF{
    namespace VIMUFF {

        class Player
        {
        public:
            Player(){ bufferSize = 5; }
            int GetNumChannels(){ return numChannels; }
            void SetBufferSize(int _size){ bufferSize = _size; }

        protected:
            int numChannels;
            int bufferSize;
        };
    }
}

#endif // PLAYER_H
