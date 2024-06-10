#include "uypch.h"
#include "Uneye/Core/ErrorWindow.h"

#include <Windows.h>
#include <cwchar> 

namespace Uneye
{
	static std::wstring stringToWString(const std::string& str) {
		std::size_t size = std::mbstowcs(nullptr, str.c_str(), 0) + 1; // +1 para o caractere nulo
		std::wstring wstr(size, L'\0');
		std::mbstowcs(&wstr[0], str.c_str(), size);
		return wstr;
	}

	void ErrorWindow::Show(const std::string& p_message, const std::string& p_title)
	{
		std::wstring message = stringToWString(p_message);
		std::wstring title = stringToWString(p_title);

		MessageBox(
			NULL,             // Handle para a janela pai, NULL se não houver
			message.c_str(),  // Mensagem a ser exibida
			title.c_str(),           // Título da caixa de diálogo
			MB_ICONERROR | MB_OK  // Ícones de erro e botão OK
		);
	}
}
