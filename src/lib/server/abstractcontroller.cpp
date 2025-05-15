#include "abstractcontroller.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

namespace RestLink {

/*!
 * \class AbstractController
 * \brief Defines the base interface for all server-side controllers.
 *
 * This class provides a contract for handling incoming server requests.
 * Subclasses must implement the `endpoint()`, `canProcessRequest()` and `processRequest()` methods.
 * A generic data source can be associated with the controller using `setDataSource()`,
 * the RestLink SQL plugin will set a QSqlDatabase instance for example.
 */

AbstractController::AbstractController()
    : m_dataSource(nullptr)
{
}

AbstractController::~AbstractController()
{
}

/*!
 * \fn virtual QString AbstractController::endpoint() const
 * \brief Returns the base endpoint this controller is responsible for.
 */

/*!
 * \brief Determines whether the controller can process the given request.
 */
bool AbstractController::canProcessRequest(const ServerRequest &request) const
{
    return request.endpoint() == endpoint();
}

/*!
 * \fn virtual void AbstractController::processRequest(const ServerRequest &request, ServerResponse *response)
 * \brief Processes the incoming request and writes the response.
 */

/*!
 * \brief Returns the data source associated with this controller.
 */
void *AbstractController::dataSource() const
{
    return m_dataSource;
}

/*!
 * \brief Sets the data source for the controller.
 *
 * \param source A generic pointer to a data source, typically used by the controller
 *               to retrieve or manipulate data.
 */
void AbstractController::setDataSource(void *source)
{
    m_dataSource = source;
}

}
