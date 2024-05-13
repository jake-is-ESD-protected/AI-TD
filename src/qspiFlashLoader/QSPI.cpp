#include "daisy_seed.h"

using namespace daisy;

DaisySeed hw;

// clang-format off
const uint64_t __attribute__((aligned(32))) data2Write[57] = {
  0x3f6000afbed8726aU, 0xbdea8ed43e72d20cU, 0x3db16462bf134beaU, 0x3e59d923be966cf3U,
  0x3df2d6713ed55777U, 0xbd6296f0bebc8112U, 0xbef3a116be265e2cU, 0xbec2cfd73edeeba4U,
  0xbdbebf9c00000000U, 0xbf13999eU, 0x3e04432d00000000U, 0x3e96b9c100000000U,
  0x3ea66382bf1fd015U, 0x0U, 0x0U, 0x3e8a13cfU,
  0xbf8c5a55bf9c75e8U, 0xbf5bb814bf787d85U, 0xbef48016bf410b89U, 0xbea51632becb318eU,
  0xbd919736be4d42a9U, 0x3e1b3e253d16bbe6U, 0x3eabab773e7b4728U, 0x3f22d4363ed46ac8U,
  0x9ace79767ab6bb8aU, 0xcdeb68b9ab667dd7U, 0xa878bd9e68b8ab87U, 0xb7c9b64da87d07dcU,
  0x7b9a8cf5d778f78cU, 0xd8b9a95a8de9d77dU, 0x6897abb89878bc89U, 0x979be63c6d8e3aebU,
  0x9da7beb8aacb96eeU, 0xb6a69d89caa786c9U, 0x6ba9c9a68677cdd8U, 0xb66bebe589b78a8bU,
  0x8b76b7998bd76899U, 0xcbc996edcca6a8a9U, 0xab87d8a58887c97cU, 0x8999a9ac78daed78U,
  0xbd6bb51300000000U, 0x3cb6bdfbbd865752U, 0xbe46e962bf20699aU, 0x3c043dee00000000U,
  0xbe66bc52U, 0xbe77a24600000000U, 0xbe804fdc00000000U, 0xbd7d75f7bec864b0U,
  0x3f11e62fbe8c62deU, 0x3f99cec53d151f08U, 0x3e55e3e34001a449U, 0x3fa6b1083ef64c28U,
  0xbeb57800bee55cabU, 0x3dd37a793eaf2412U, 0xbece3c063eb89b18U, 0x3ee8a08c3fa3cfe6U,
  0xbea79a2bU,
};

uint64_t DSY_QSPI_BSS qspi_buffer[57];


int main(void)
{
    hw.Configure();
    hw.Init();

    size_t size = sizeof(data2Write);
    /** Grab physical address from pointer */
    size_t address = (size_t)qspi_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t*)data2Write);


    while(1)
    {
        hw.SetLed(true);
        System::Delay(1000);
        hw.SetLed(false);
        System::Delay(1000);
    }
}