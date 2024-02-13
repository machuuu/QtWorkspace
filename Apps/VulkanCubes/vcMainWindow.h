#include <QWidget>

namespace vc 
{
	class MainWindow : public QWidget
	{
		Q_OBJECT

	public:
		
		//MainWindow(VulkanWindow* vkWin);
		MainWindow();
		virtual ~MainWindow();

		MainWindow(const MainWindow&) = delete;
		MainWindow& operator=(const MainWindow& x) = delete;
		MainWindow(MainWindow&&) = delete;
		MainWindow& operator=(MainWindow&& other) = delete;		

	private:

		void connectUIComponents(); // better name?

		//VulkanWindow* m_vkWin;

	};
}