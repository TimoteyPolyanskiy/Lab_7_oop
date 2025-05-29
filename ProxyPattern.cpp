#include "ProxyPattern.h"


class IRemoteService {
public:
    virtual ~IRemoteService() = default;
    virtual std::string getSensitiveData(const std::string& query) = 0;
    virtual bool performAdminAction(const std::string& action) = 0;
};

class RealRemoteService : public IRemoteService {
private:
    bool connected;
    void simulateConnection() {
        std::cout << "RealRemoteService: Спроба підключення до віддаленого сервера..." << std::endl;
        if ((rand() % 10) > 2) {
            connected = true;
            std::cout << "RealRemoteService: З'єднання встановлено." << std::endl;
        } else {
            connected = false;
            std::cout << "RealRemoteService: Не вдалося підключитися." << std::endl;
        }
    }

public:
    RealRemoteService() : connected(false) {
        std::cout << "RealRemoteService: Екземпляр створено." << std::endl;
    }

    std::string getSensitiveData(const std::string& query) override {
        if (!connected) simulateConnection();
        if (!connected) return "Помилка: Немає з'єднання з віддаленим сервісом.";

        std::cout << "RealRemoteService: Обробка запиту '" << query << "'." << std::endl;
        return "Конфіденційні дані для запиту: " + query;
    }

    bool performAdminAction(const std::string& action) override {
        if (!connected) simulateConnection();
        if (!connected) {
            std::cout << "RealRemoteService: Неможливо виконати дію адміністратора '" << action << "'. Немає з'єднання." << std::endl;
            return false;
        }
        std::cout << "RealRemoteService: Виконання дії адміністратора '" << action << "'." << std::endl;
        return true;
    }
};

class AccessDeniedException : public std::runtime_error {
public:
    AccessDeniedException(const std::string& message) : std::runtime_error(message) {}
};

class ConnectionFailedException : public std::runtime_error {
public:
    ConnectionFailedException(const std::string& message) : std::runtime_error(message) {}
};

class ServiceProxy : public IRemoteService {
private:
    RealRemoteService* realService;
    std::string userRole;

    void ensureServiceInitialized() {
        if (realService == nullptr) {
            std::cout << "ServiceProxy: Ініціалізація RealRemoteService (відкладена ініціалізація)..." << std::endl;
            realService = new RealRemoteService();
        }
    }

public:
    ServiceProxy(const std::string& role) : realService(nullptr), userRole(role) {
        std::cout << "ServiceProxy: Екземпляр створено для ролі користувача '" << userRole << "'." << std::endl;
        srand(static_cast<unsigned int>(time(0))); // Ініціалізація один раз на початку
    }

    ~ServiceProxy() override {
        delete realService;
        std::cout << "ServiceProxy: Екземпляр знищено. Реальний сервіс звільнено, якщо був створений." << std::endl;
    }

    std::string getSensitiveData(const std::string& query) override {
        ensureServiceInitialized();
        std::cout << "ServiceProxy: Користувач '" << userRole << "' намагається отримати дані для запиту '" << query << "'." << std::endl;
        std::string result = realService->getSensitiveData(query);
        if (result.find("Помилка: Немає з'єднання") != std::string::npos) {
            throw ConnectionFailedException("Проксі: Не вдалося отримати дані через проблеми зі з'єднанням.");
        }
        return result;
    }

    bool performAdminAction(const std::string& action) override {
        ensureServiceInitialized();
        std::cout << "ServiceProxy: Користувач '" << userRole << "' намагається виконати дію адміністратора '" << action << "'." << std::endl;
        if (userRole != "admin") {
            std::cerr << "ServiceProxy: Доступ заборонено! Користувач '" << userRole << "' не може виконувати дії адміністратора." << std::endl;
            throw AccessDeniedException("Користувач не має прав адміністратора.");
        }
        bool success = realService->performAdminAction(action);
        if (!success && realService->getSensitiveData("check_status").find("Помилка: Немає з'єднання") != std::string::npos) {
             throw ConnectionFailedException("Проксі: Не вдалося виконати дію адміністратора через проблеми зі з'єднанням.");
        }
        return success;
    }
};

void demonstrateProxy() {
    std::cout << "\n=== Демонстрація шаблону Замісник ===\n\n";

    IRemoteService* userService = new ServiceProxy("user");
    try {
        std::cout << "--- Доступ користувача ---" << std::endl;
        std::cout << "Дані: " << userService->getSensitiveData("user_profile") << std::endl;
        std::cout << "Спроба виконати дію адміністратора (має бути відмовлено або помилка):" << std::endl;
        userService->performAdminAction("delete_database");
    } catch (const AccessDeniedException& e) {
        std::cerr << "Перехоплено виняток: " << e.what() << std::endl;
    } catch (const ConnectionFailedException& e) {
        std::cerr << "Перехоплено виняток з'єднання: " << e.what() << std::endl;
    }
     catch (const std::exception& e) {
        std::cerr << "Перехоплено неочікуваний виняток: " << e.what() << std::endl;
    }
    delete userService;
    std::cout << std::endl;

    IRemoteService* adminService = new ServiceProxy("admin");
    try {
        std::cout << "--- Доступ адміністратора ---" << std::endl;
        std::cout << "Дані: " << adminService->getSensitiveData("all_user_data") << std::endl;
        std::cout << "Спроба виконати дію адміністратора (має бути успішно, якщо є з'єднання):" << std::endl;
        if (adminService->performAdminAction("restart_server")) {
            std::cout << "Дія адміністратора 'restart_server' успішна." << std::endl;
        } else {
            std::cout << "Дія адміністратора 'restart_server' не вдалася (можливо, немає з'єднання або інша проблема)." << std::endl;
        }
    } catch (const AccessDeniedException& e) {
        std::cerr << "Перехоплено виняток: " << e.what() << std::endl;
    } catch (const ConnectionFailedException& e) {
        std::cerr << "Перехоплено виняток з'єднання: " << e.what() << std::endl;
    }
     catch (const std::exception& e) {
        std::cerr << "Перехоплено неочікуваний виняток: " << e.what() << std::endl;
    }
    delete adminService;

    std::cout << "\n=== Кінець демонстрації Замісника ===\n";
}