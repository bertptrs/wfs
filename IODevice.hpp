#ifndef IODEVICE_HPP
#define IODEVICE_HPP

namespace wfs {
	class IODevice {
		public:
			virtual ~IODevice() = default;

			virtual void read(size_t offset, size_t amount, void* buff) = 0;
			virtual void write(size_t offset, size_t amount, const void* buff) = 0;

			template<class T>
				void read(size_t offset, T& buff)
				{
					read(offset, sizeof(T), &buff);
				}

			template<class T>
				void write(size_t offset, const T& buff)
				{
					write(offset, sizeof(T), &buff);
				}
	};
}

#endif
